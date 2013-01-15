/*****************************************************************************
 * macroblock.c: macroblock common functions
 *****************************************************************************
 * Copyright (C) 2003-2010 x264 project
 *
 * Authors: Jason Garrett-Glaser <darkshikari@gmail.com>
 *          Laurent Aimar <fenrir@via.ecp.fr>
 *          Loren Merritt <lorenm@u.washington.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#include "common.h"
#include "encoder/me.h"

static NOINLINE void dull_mb_mc_0xywh( x264_t *h, int x, int y, int width, int height )
{
    int i8    = x264_scan8[0]+x+8*y;
    int i_ref = h->mb.cache.ref[0][i8];
    int mvx   = x264_clip3( h->mb.cache.mv[0][i8][0], h->mb.mv_min[0], h->mb.mv_max[0] ) + 4*4*x;
    int mvy   = x264_clip3( h->mb.cache.mv[0][i8][1], h->mb.mv_min[1], h->mb.mv_max[1] ) + 4*4*y;

    h->mc.mc_luma( &h->mb.pic.p_fdec[0][4*y*FDEC_STRIDE+4*x], FDEC_STRIDE,
                   h->mb.pic.p_fref[0][i_ref], h->mb.pic.i_stride[0],
                   mvx, mvy, 4*width, 4*height, &h->sh.weight[i_ref][0] );

    // chroma is offset if MCing from a field of opposite parity
    if( h->mb.b_interlaced & i_ref )
        mvy += (h->mb.i_mb_y & 1)*4 - 2;

    h->mc.mc_chroma( &h->mb.pic.p_fdec[1][2*y*FDEC_STRIDE+2*x],
                     &h->mb.pic.p_fdec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                     h->mb.pic.p_fref[0][i_ref][4], h->mb.pic.i_stride[1],
                     mvx, mvy, 2*width, 2*height );

    if( h->sh.weight[i_ref][1].weightfn )
        h->sh.weight[i_ref][1].weightfn[width>>1]( &h->mb.pic.p_fdec[1][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                                                   &h->mb.pic.p_fdec[1][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                                                   &h->sh.weight[i_ref][1], height*2 );
    if( h->sh.weight[i_ref][2].weightfn )
        h->sh.weight[i_ref][2].weightfn[width>>1]( &h->mb.pic.p_fdec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                                                   &h->mb.pic.p_fdec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                                                   &h->sh.weight[i_ref][2],height*2 );

}
static NOINLINE void dull_mb_mc_1xywh( x264_t *h, int x, int y, int width, int height )
{
    int i8    = x264_scan8[0]+x+8*y;
    int i_ref = h->mb.cache.ref[1][i8];
    int mvx   = x264_clip3( h->mb.cache.mv[1][i8][0], h->mb.mv_min[0], h->mb.mv_max[0] ) + 4*4*x;
    int mvy   = x264_clip3( h->mb.cache.mv[1][i8][1], h->mb.mv_min[1], h->mb.mv_max[1] ) + 4*4*y;

    h->mc.mc_luma( &h->mb.pic.p_fdec[0][4*y*FDEC_STRIDE+4*x], FDEC_STRIDE,
                   h->mb.pic.p_fref[1][i_ref], h->mb.pic.i_stride[0],
                   mvx, mvy, 4*width, 4*height, weight_none );

    if( h->mb.b_interlaced & i_ref )
        mvy += (h->mb.i_mb_y & 1)*4 - 2;

    h->mc.mc_chroma( &h->mb.pic.p_fdec[1][2*y*FDEC_STRIDE+2*x],
                     &h->mb.pic.p_fdec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                     h->mb.pic.p_fref[1][i_ref][4], h->mb.pic.i_stride[1],
                     mvx, mvy, 2*width, 2*height );
}

static NOINLINE void dull_mb_mc_01xywh( x264_t *h, int x, int y, int width, int height )
{
    int i8 = x264_scan8[0]+x+8*y;
    int i_ref0 = h->mb.cache.ref[0][i8];
    int i_ref1 = h->mb.cache.ref[1][i8];
    int weight = h->mb.bipred_weight[i_ref0][i_ref1];
    int mvx0   = x264_clip3( h->mb.cache.mv[0][i8][0], h->mb.mv_min[0], h->mb.mv_max[0] ) + 4*4*x;
    int mvx1   = x264_clip3( h->mb.cache.mv[1][i8][0], h->mb.mv_min[0], h->mb.mv_max[0] ) + 4*4*x;
    int mvy0   = x264_clip3( h->mb.cache.mv[0][i8][1], h->mb.mv_min[1], h->mb.mv_max[1] ) + 4*4*y;
    int mvy1   = x264_clip3( h->mb.cache.mv[1][i8][1], h->mb.mv_min[1], h->mb.mv_max[1] ) + 4*4*y;
    int i_mode = x264_size2pixel[height][width];
    int i_stride0 = 16, i_stride1 = 16;
    ALIGNED_ARRAY_16( pixel, tmp0,[16*16] );
    ALIGNED_ARRAY_16( pixel, tmp1,[16*16] );
    pixel *src0, *src1;

    src0 = h->mc.get_ref( tmp0, &i_stride0, h->mb.pic.p_fref[0][i_ref0], h->mb.pic.i_stride[0],
                          mvx0, mvy0, 4*width, 4*height, weight_none );
    src1 = h->mc.get_ref( tmp1, &i_stride1, h->mb.pic.p_fref[1][i_ref1], h->mb.pic.i_stride[0],
                          mvx1, mvy1, 4*width, 4*height, weight_none );
    h->mc.avg[i_mode]( &h->mb.pic.p_fdec[0][4*y*FDEC_STRIDE+4*x], FDEC_STRIDE,
                       src0, i_stride0, src1, i_stride1, weight );

    if( h->mb.b_interlaced & i_ref0 )
        mvy0 += (h->mb.i_mb_y & 1)*4 - 2;
    if( h->mb.b_interlaced & i_ref1 )
        mvy1 += (h->mb.i_mb_y & 1)*4 - 2;

    h->mc.mc_chroma( tmp0, tmp0+8, 16, h->mb.pic.p_fref[0][i_ref0][4], h->mb.pic.i_stride[1],
                     mvx0, mvy0, 2*width, 2*height );
    h->mc.mc_chroma( tmp1, tmp1+8, 16, h->mb.pic.p_fref[1][i_ref1][4], h->mb.pic.i_stride[1],
                     mvx1, mvy1, 2*width, 2*height );
    h->mc.avg[i_mode+3]( &h->mb.pic.p_fdec[1][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE, tmp0, 16, tmp1, 16, weight );
    h->mc.avg[i_mode+3]( &h->mb.pic.p_fdec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE, tmp0+8, 16, tmp1+8, 16, weight );
}

void dull_mb_mc_8x8( x264_t *h, int i8 )
{
    int x = 2*(i8&1);
    int y = 2*(i8>>1);

    if( h->sh.i_type == SLICE_TYPE_P )
    {
        switch( h->mb.i_sub_partition[i8] )
        {
            case D_L0_8x8:
                dull_mb_mc_0xywh( h, x, y, 2, 2 );
                break;
            case D_L0_8x4:
                dull_mb_mc_0xywh( h, x, y+0, 2, 1 );
                dull_mb_mc_0xywh( h, x, y+1, 2, 1 );
                break;
            case D_L0_4x8:
                dull_mb_mc_0xywh( h, x+0, y, 1, 2 );
                dull_mb_mc_0xywh( h, x+1, y, 1, 2 );
                break;
            case D_L0_4x4:
                dull_mb_mc_0xywh( h, x+0, y+0, 1, 1 );
                dull_mb_mc_0xywh( h, x+1, y+0, 1, 1 );
                dull_mb_mc_0xywh( h, x+0, y+1, 1, 1 );
                dull_mb_mc_0xywh( h, x+1, y+1, 1, 1 );
                break;
        }
    }
    else
    {
        int scan8 = x264_scan8[0] + x + 8*y;

        if( h->mb.cache.ref[0][scan8] >= 0 )
            if( h->mb.cache.ref[1][scan8] >= 0 )
                dull_mb_mc_01xywh( h, x, y, 2, 2 );
            else
                dull_mb_mc_0xywh( h, x, y, 2, 2 );
        else
            dull_mb_mc_1xywh( h, x, y, 2, 2 );
    }
}

void dull_mb_mc( x264_t *h )
{
    if( h->mb.i_partition == D_8x8 )
    {
        int i;
        for( i = 0; i < 4; i++ )
            dull_mb_mc_8x8( h, i );
    }
    else
    {
        int ref0a = h->mb.cache.ref[0][x264_scan8[ 0]];
        int ref0b = h->mb.cache.ref[0][x264_scan8[12]];
        int ref1a = h->mb.cache.ref[1][x264_scan8[ 0]];
        int ref1b = h->mb.cache.ref[1][x264_scan8[12]];

        if( h->mb.i_partition == D_16x16 )
        {
            if( ref0a >= 0 )
                if( ref1a >= 0 ) dull_mb_mc_01xywh( h, 0, 0, 4, 4 );
                else             dull_mb_mc_0xywh ( h, 0, 0, 4, 4 );
            else                 dull_mb_mc_1xywh ( h, 0, 0, 4, 4 );
        }
        else if( h->mb.i_partition == D_16x8 )
        {
            if( ref0a >= 0 )
                if( ref1a >= 0 ) dull_mb_mc_01xywh( h, 0, 0, 4, 2 );
                else             dull_mb_mc_0xywh ( h, 0, 0, 4, 2 );
            else                 dull_mb_mc_1xywh ( h, 0, 0, 4, 2 );

            if( ref0b >= 0 )
                if( ref1b >= 0 ) dull_mb_mc_01xywh( h, 0, 2, 4, 2 );
                else             dull_mb_mc_0xywh ( h, 0, 2, 4, 2 );
            else                 dull_mb_mc_1xywh ( h, 0, 2, 4, 2 );
        }
        else if( h->mb.i_partition == D_8x16 )
        {
            if( ref0a >= 0 )
                if( ref1a >= 0 ) dull_mb_mc_01xywh( h, 0, 0, 2, 4 );
                else             dull_mb_mc_0xywh ( h, 0, 0, 2, 4 );
            else                 dull_mb_mc_1xywh ( h, 0, 0, 2, 4 );

            if( ref0b >= 0 )
                if( ref1b >= 0 ) dull_mb_mc_01xywh( h, 2, 0, 2, 4 );
                else             dull_mb_mc_0xywh ( h, 2, 0, 2, 4 );
            else                 dull_mb_mc_1xywh ( h, 2, 0, 2, 4 );
        }
    }
}
