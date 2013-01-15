// millet.c

#include "mcdull_core.h"
#include "common/common.h"
#include "encoder/ratecontrol.h"

#include "millet.h"



MILLET_GLOBAL
void millet_init_preprocessor(void *p, int rotate_mode, int zoom_mode, int yuv_format, int yuv_offset)
{
	millet_encoder_t *e = (millet_encoder_t *)p;
	e->rotate_mode = rotate_mode;
	e->zoom_mode = zoom_mode;
	e->yuv_format = yuv_format;
	e->yuv_offset = yuv_offset;
}


MILLET_GLOBAL
void millet_exit_files(void *p)
{
	millet_encoder_t *e = (millet_encoder_t *)p;

	if(!e)
		return ;

#if MILLET_SOURCE_YUV_FILE
	if(e->file_f_yuv)
	{
		fclose(e->file_f_yuv);
		e->file_f_yuv = NULL;
		memset (e->filename_f_yuv, 0, MILLET_MAX_FILE_NAME_SIZE);
	}
#endif//MILLET_SOURCE_YUV_FILE

#if	MILLET_264_STREAM_FILE
	if(e->file_g_264)
	{
		fclose(e->file_g_264);
		e->file_g_264 = NULL;
		memset (e->filename_g_264, 0, MILLET_MAX_FILE_NAME_SIZE);
	}
#endif//MILLET_264_STREAM_FILE

#if MILLET_264_INFO_FILE
	if(e->file_info)
	{
		fclose(e->file_info);
		e->file_info = NULL;
		memset (e->filename_info, 0, MILLET_MAX_FILE_NAME_SIZE);
	}
#endif

#if MILLET_DECODED_YUV_FILE
	if(e->file_g_yuv)
	{
		fclose(e->file_g_yuv);
		e->file_g_yuv = NULL;
		memset (e->filename_g_yuv, 0, MILLET_MAX_FILE_NAME_SIZE);
	}
#endif
}


//Note: filename is without subfix name
MILLET_GLOBAL
int millet_init_files(void *p, char *filename, int write_source_yuv_file)
{
	millet_encoder_t *e = (millet_encoder_t *)p;

	// 1 for write or 0 for read
	e->write_source_yuv_file_flag = write_source_yuv_file;

#if MILLET_SOURCE_YUV_FILE
	sprintf(e->filename_f_yuv, "%s.yuv" , filename);
	{
		char *open_mode = e->write_source_yuv_file_flag ? "wb" : "rb";
		e->file_f_yuv = fopen(e->filename_f_yuv, open_mode);	
	}
	if (!e->file_f_yuv)
	{
		millet_exit_files(e);
		return -1;
	}
#endif//MILLET_SOURCE_YUV_FILE

#if MILLET_264_STREAM_FILE
	sprintf(e->filename_g_264, "%s_millet.264", filename);
	e->file_g_264 = fopen(e->filename_g_264, "wb");
	if (!e->file_g_264)
	{
		millet_exit_files(e);
		return -2;
	}
#endif//MILLET_264_STREAM_FILE

#if MILLET_264_INFO_FILE
	sprintf(e->filename_info , "%s_millet.txt", filename);
	e->file_info  = fopen(e->filename_info , "wt");
	if (!e->file_info )
	{
		millet_exit_files(e);
		return -3;
	}
	fprintf(e->file_info, " width    = %6d\n"    , e->width);
	fprintf(e->file_info, " height   = %6d\n"    , e->height);
	fprintf(e->file_info, " framerate= %6dfps\n" , e->fps);
	fprintf(e->file_info, " bitrate  = %6dkbps\n", e->bitrate);
	fprintf(e->file_info, " goplength= %6d\n"    , e->gop_length);
	fprintf(e->file_info, "\n\n\n");
#endif

#if MILLET_DECODED_YUV_FILE
	sprintf(e->filename_g_yuv, "%s_millet.yuv", filename);
	e->file_g_yuv = fopen(e->filename_g_yuv, "wb");
	if (!e->file_g_yuv)
	{
		millet_exit_files(e);
		return -4;
	}
#endif

	return 0;
}


MILLET_LOCAL
void millet_output_frame(millet_encoder_t *e)
{
	int g_264_size = e->g_264[0];
	if ( g_264_size > 0)
	{
#if	MILLET_264_STREAM_FILE
		fwrite(&e->g_264[1], e->g_264[0], 1, e->file_g_264);
#endif

#if	MILLET_264_INFO_FILE
		fprintf(e->file_info, " frame = %6d, ", e->frame_counter);
		fprintf(e->file_info, " size = %6d\n" , g_264_size);
#endif

#if MILLET_DECODED_YUV_FILE
		fwrite(e->g_yuv, e->yuv_size, 1, e->file_g_yuv);
#endif

		e->frame_counter++;
	}
}


MILLET_GLOBAL
int millet_read_source_yuv_file(void *p)
{
	int read_bytes = 0;
#if MILLET_SOURCE_YUV_FILE
	millet_encoder_t *e = (millet_encoder_t *)p;
	read_bytes = fread(e->f_yuv, e->yuv_size, 1, e->file_f_yuv);
#endif
	return read_bytes;
}


MILLET_GLOBAL
void millet_write_source_yuv_file(void *p)
{
#if MILLET_SOURCE_YUV_FILE
	millet_encoder_t *e = (millet_encoder_t *)p;
	fwrite(e->f_yuv, e->yuv_size, 1, e->file_f_yuv);
#endif
}


MILLET_LOCAL
int millet_open(millet_encoder_t *e)
{
#if MILLET_FASTEST_SETTING
    char* preset  = "superfast";
    char* profile = "main";
#else
	char* preset  = NULL;
    char* profile = NULL;
#endif

    McDull_t* m = (McDull_t*)e->mcdull;
	x264_param_t* param = m->param;

	int width = e->width;
	int height= e->height; 

    m->width  = width;
    m->height = height;
	x264_picture_init(m->pic);
    x264_picture_alloc(m->pic, X264_CSP_I420, width, height);
    x264_picture_init (m->pic_out);
    x264_picture_alloc(m->pic_out, X264_CSP_I420, width, height);

    x264_param_default_preset( param, preset, NULL );
	x264_param_apply_fastfirstpass( param );
    x264_param_apply_profile( param, profile );
    param->i_width  = width;
    param->i_height = height;

    //OPT("keyint")
    {
        param->i_keyint_min =
		param->i_keyint_max = m->keyint;
    }
    //OPT("bframes")
    {
        param->i_bframe = m->bframe;
    }

	if(e->fps_control_flag)
	{
		param->i_fps_num = e->fps;
		param->i_fps_den = 1;
	}
	
	if(e->rate_control_flag)//OPT("bitrate")
    {
        param->rc.i_bitrate = e->bitrate;
        param->rc.i_rc_method = X264_RC_ABR;
    }
	else//OPT2("qp", "qp_constant")
    {
        param->rc.i_qp_constant = m->qp;
        param->rc.i_rc_method = X264_RC_CQP;
    }

    m->x264 = x264_encoder_open(param);
    if( m->x264 == NULL ) return -1;

    x264_encoder_parameters(m->x264, param);


#if mcdull_SWATAW
	m->swataw = swataw_new();
    swataw_open(m->swataw, width, height);
#endif

#if 0
	// init ff_cropTbl
	{
		int i;
		for(i=0;i<256;i++)
			ff_cropTbl[i + 1024] = i;    
		for(i=0;i<1024;i++)
		{
			ff_cropTbl[i] = 0;
			ff_cropTbl[i + 1024 + 256] = 255;
		}
	}
#endif

    return 0;
}


MILLET_LOCAL
void millet_close(millet_encoder_t *e)
{
    McDull_t* m = e->mcdull;

#if mcdull_SWATAW
    swataw_close(m->swataw);
	swataw_delete(m->swataw);
#endif

    x264_encoder_close(m->x264);
}

extern void picNV12_to_yuvI420(x264_picture_t* pic, int l1, int l2, uint8_t* yuv);
extern void yuvI420_to_picI420(uint8_t* yuv, int l1, int l2, x264_picture_t* pic);

MILLET_LOCAL
int millet_encode(McDull_t* m,
        uint8_t f_yuv[], 
        int pts,
        uint8_t g_264[], int32_t g_264_size[1], 
        uint8_t g_yuv[])
{
    int width  = m->width;
    int height = m->height;
    int i_frame_size = 0;

    x264_picture_t* f_pic = m->pic;
    x264_picture_t* g_pic = m->pic_out;

    yuvI420_to_picI420(f_yuv, width, height, f_pic);

    f_pic->i_pts = pts;

    i_frame_size = dull_encoder_encode( m, &m->p_nal, &m->i_nal, f_pic, g_pic );
    g_264_size[0] = i_frame_size;

    if (i_frame_size < 0) return -1;

    if( i_frame_size > 0 )
    {
        memcpy(g_264, m->p_nal[0].p_payload, i_frame_size);
		if(g_yuv)
			picNV12_to_yuvI420(g_pic, width, height, g_yuv);
    }

    return i_frame_size;
}


MILLET_LOCAL
int millet_encode_flush(McDull_t* m, uint8_t g_264[], int32_t g_264_size[1], uint8_t g_yuv[])
{
    int width  = m->width;
    int height = m->height;

	int i_frame_size = 0;

    x264_picture_t* g_pic = m->pic_out;

    i_frame_size = dull_encoder_encode( m, &m->p_nal, &m->i_nal, NULL, g_pic );

    g_264_size[0] = i_frame_size;
    if( i_frame_size > 0 )
    {
        memcpy(g_264, m->p_nal[0].p_payload, i_frame_size);
        if(g_yuv)
			picNV12_to_yuvI420(g_pic, width, height, g_yuv);
	    m->frame_out++;
    }
    if (i_frame_size < 0) return -1;

    return i_frame_size;
}


MILLET_GLOBAL
int millet_encoder_encode(void *p)
{
	millet_encoder_t *e = (millet_encoder_t *)p;
	int frame_size = millet_encode(e->mcdull, e->f_yuv,
		e->frame_counter, (uint8_t *)&e->g_264[1], &e->g_264[0], e->g_yuv);
	millet_output_frame(e);
	return frame_size;
}


MILLET_GLOBAL
int millet_encoder_encode_flush(void *p)
{
	millet_encoder_t *e = (millet_encoder_t *)p;
	int frame_size = millet_encode_flush(e->mcdull, (uint8_t *)&e->g_264[1], &e->g_264[0], e->g_yuv);;
	millet_output_frame(e);
	return frame_size;
}


MILLET_GLOBAL
void millet_exit_encoder(void *p)
{
	millet_encoder_t *e = (millet_encoder_t *)p;
	millet_close ((void *)e);
	mcdull_delete((void *)e->mcdull);
	if(e->f_yuv){	free(e->f_yuv);		e->f_yuv = NULL;	}
	if(e->g_yuv){	free(e->g_yuv);		e->g_yuv = NULL;	}
	if(e->g_264){	free(e->g_264);		e->g_264 = NULL;	}
}


MILLET_GLOBAL
int millet_init_encoder(void *p, int width, int height, int fps, int bitrate, int gop_length)
{
	millet_encoder_t *e = (millet_encoder_t *)p;

	memset(e, 0, sizeof(millet_encoder_t));

	e->width = width;
	e->height = height;
	e->yuv_size = (width * height * 3) >> 1;

	e->fps = fps;

	e->bitrate = bitrate;

	e->gop_length = gop_length;
	e->frame_counter=0;

	e->f_yuv = malloc( e->yuv_size );	if(!e->f_yuv){	millet_exit_encoder(p);		return -1;	}
	e->g_yuv = malloc( e->yuv_size );	if(!e->g_yuv){	millet_exit_encoder(p);		return -2;	}
	e->g_264 = malloc( e->yuv_size );	if(!e->g_264){	millet_exit_encoder(p);		return -3;	}
	e->mcdull = mcdull_new();			if(!e->mcdull){	millet_exit_encoder(p);		return -4;	}

	mcdull_set_qp    (e->mcdull, 28);
	mcdull_set_keyint(e->mcdull, e->gop_length);

#if MILLET_FASTEST_SETTING
	e->fps_control_flag=1;
	e->rate_control_flag=1;
	mcdull_set_bframe(e->mcdull, 0);
#else
	e->fps_control_flag=0;
	e->rate_control_flag=0;
	mcdull_set_bframe(e->mcdull, 1);
#endif

	if( millet_open(e) < 0)
	{
		millet_exit_encoder(p);
		return -5;
	}

	return 0;
}


#if	MILLET_264_STREAM_FILE
MILLET_GLOBAL
int millet_set_264_file_name(void *p, char *avc_file_name)
{
	millet_encoder_t *e = (millet_encoder_t *)p;

	char *fn = e->filename_g_264;
	memset (fn, 0, MILLET_MAX_FILE_NAME_SIZE);
	sprintf(fn, avc_file_name);

	if (e->file_g_264)
	{
		fclose(e->file_g_264);
		e->file_g_264 = NULL;
	}

	e->file_g_264 = fopen(fn, "wb");
	if (!e->file_g_264)
		return -1;

	return 0;
}
#endif//MILLET_264_STREAM_FILE



MILLET_GLOBAL
void millet_show_mean_time(void *p, double total_time)
{
	millet_encoder_t *e = (millet_encoder_t *)p;

#if	MILLET_264_INFO_FILE
	fprintf(e->file_info, " total %d frames\n", e->frame_counter);
	fprintf(e->file_info, " mean time = %f frame per second\n", total_time/e->frame_counter);
#endif

}

int g_counter = 0;
int millet_get_counter()
{
	return g_counter;
}



