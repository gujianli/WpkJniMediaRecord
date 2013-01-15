#include "mcdull_core.h"

#include "encoder/macroblock.h"

void McDull_swataw_P_SKIP( McDull_t* McDull )
{
    x264_t* h = McDull->x264;

    int i;

    h->mb.i_type = P_SKIP;
    h->mb.i_partition = D_16x16;

    x264_plane_copy_c(
        h->mb.pic.p_fdec[0], FDEC_STRIDE,
        h->mb.pic.p_fref[0][0][0], h->mb.pic.i_stride[0],
        16, 16 );

    x264_plane_copy_deinterleave_c(
        h->mb.pic.p_fdec[1],               FDEC_STRIDE,
        h->mb.pic.p_fdec[1]+FDEC_STRIDE/2, FDEC_STRIDE,
        h->mb.pic.p_fref[0][0][4], h->mb.pic.i_stride[1],
        8, 8);

    M32( h->mb.mvr[0][0][h->mb.i_mb_xy] ) = 0;

    x264_macroblock_cache_ref   ( h, 0, 0, 4, 4, 0, 0 );
    x264_macroblock_cache_mv_ptr( h, 0, 0, 4, 4, 0, h->mb.cache.pskip_mv );

    M32( &h->mb.cache.non_zero_count[x264_scan8[0]+0*8] ) = 0;
    M32( &h->mb.cache.non_zero_count[x264_scan8[0]+1*8] ) = 0;
    M32( &h->mb.cache.non_zero_count[x264_scan8[0]+2*8] ) = 0;
    M32( &h->mb.cache.non_zero_count[x264_scan8[0]+3*8] ) = 0;
    for( i = 16; i < 24; i++ )
        h->mb.cache.non_zero_count[x264_scan8[i]] = 0;

    h->mb.i_cbp_luma = 0;
    h->mb.i_cbp_chroma = 0;
    h->mb.cbp[h->mb.i_mb_xy] = 0;
}
