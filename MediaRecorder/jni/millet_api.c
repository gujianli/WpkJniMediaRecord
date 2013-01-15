// millet_api.c

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>


#include "mcdull/mcdull.h"
#include "mcdull/millet.h"

millet_encoder_t g_encoder;
millet_encoder_t *gpec = &g_encoder;


// write some info for special cellphone of android only once
int millet_write_preview_info(int fps, int yuv_interval)
{
	FILE *file_txt=NULL;
	char *txt_name = "/sdcard/android/preview.txt";

	file_txt = fopen(txt_name, "wt");
	if(!file_txt)
		return -1;

	fprintf(file_txt, "frame rate = %d\n", fps);
	fprintf(file_txt, " interval  = %d\n", yuv_interval);

	fclose(file_txt);

	return 0;
}




// YUV420SP --> YUV420P
int millet_yuv420_convert_sp2p(uint8_t *yuv420sp,
		int width, int height, uint8_t *yuv420p)
{
	int  i;
	int size = (width * height * 3) >> 1;
	int size_y  = width*height;
	int size_vu = size_y >> 2;
	int size_yuv= (size_y*3)>>1;

	uint8_t *spy = yuv420sp;
	uint8_t *spvu = spy + size_y;

	uint8_t *py = yuv420p;
	uint8_t *pu = py + size_y;
	uint8_t *pv = pu + (size_y>>2);

	memcpy(py, spy, size_y);

	for(i=0; i<size_vu; i++)
	{
		*pv++ = *spvu++;
		*pu++ = *spvu++;
	}

	return 0;
}

// YUV420SP --> YUV420P
// scale down size 4x
// 2 things:
//   1)  YUV420SP --> YUV420P
//   2)  width and height of YUV420P all be 1/2 of YUV420SP
int millet_yuv420_convert_sp2p_zoomout_4x(uint8_t *yuv420sp,
		int width, int height, uint8_t *yuv420p)
{
	int w,h;
	int w4 = width>>1;
	int h4 = height>>1;
	int size_y = (width * height);

	uint8_t *spy0 = yuv420sp;
	uint8_t *spy1 = spy0 + (width<<1);
	uint8_t *spy2 = spy1 + (width<<1);
	uint8_t *spy3 = spy2 + (width<<1);
	uint8_t *spvu = spy0 + (size_y<<2);

	uint8_t *py0 = yuv420p;
	uint8_t *py1 = py0 + width;
	uint8_t *pu = py0 + size_y;
	uint8_t *pv = pu + (size_y>>2);

	for(h=0; h<h4; h++)
	{
		for(w=0; w<w4; w++)
		{
			*py0++ = ( (*spy0++) + (*spy0++) + (*spy1++) + (*spy1++) ) >> 2;
			*py0++ = ( (*spy0++) + (*spy0++) + (*spy1++) + (*spy1++) ) >> 2;
			*py1++ = ( (*spy2++) + (*spy2++) + (*spy3++) + (*spy3++) ) >> 2;
			*py1++ = ( (*spy2++) + (*spy2++) + (*spy3++) + (*spy3++) ) >> 2;

			*pv++ = ( spvu[0] + spvu[2] + spvu[(width<<1)  ] + spvu[(width<<1)+2] ) >> 2;
			*pu++ = ( spvu[1] + spvu[3] + spvu[(width<<1)+1] + spvu[(width<<1)+3] ) >> 2;
			spvu += 4;
		}

		py0  = py1;
		py1 += width;
		spy0 = spy3;
		spy1 = spy0 + (width<<1);
		spy2 = spy1 + (width<<1);
		spy3 = spy2 + (width<<1);
		spvu += width<<1;
	}

	return 0;
}

int millet_yuv420_input_HTC_G14(uint8_t *yuv420src,
		int width, int height, uint8_t *yuv420dst)
{
	// 对 HTC G14进行校正：数据偏移16bytes
	millet_yuv420_convert_sp2p(yuv420src+16, width, height, yuv420dst);
	return 0;
}


int millet_encode_frame(uint8_t *yuv420src)
{
	int g_264_size;
	if( (gpec->width<0) || (gpec->height<0) )
		return -1;

	if( (gpec->rotate_mode!=ML_ROTATE000) ||
		(gpec->zoom_mode != ML_ZOOMOUT1X) ||
		(gpec->yuv_format != ML_YUV420SP) )
		return -2;

	if( gpec->zoom_mode == ML_ZOOMOUT1X)
		millet_yuv420_input_HTC_G14(yuv420src, gpec->width, gpec->height, gpec->f_yuv);
//	millet_yuv420_convert_sp2p_zoomout_4x(yuv420src+gpec->yuv_offset, gpec->width, gpec->height, gpec->f_yuv);

	g_264_size = millet_encoder_encode(gpec);
		
	return 0;
}



#include <time.h>


int main_millet(int argc, char* argv[])
{
	int width = 352;
	int height= 288;
	int fps = 15;
	int bitrate = 100;
	int gop_length = 50;

	int rotate_mode = ML_ROTATE000;
	int zoom_mode = ML_ZOOMOUT1X;
	int yuv_format = ML_YUV420P;
	int yuv_offset =0;

#ifdef _MSC_VER
	char *filename = "paris";
#else
	char *filename = "/sdcard/android/paris";
#endif

	int f, ret;

	clock_t starttime, endtime;
	double totaltime;

	starttime = clock();


	millet_init_encoder(gpec, width, height, fps, bitrate, gop_length);
	millet_init_preprocessor(gpec, rotate_mode, zoom_mode, yuv_format, yuv_offset);
	millet_init_files(gpec, filename, 0);

    for( f=0;  ; f++ )
    {
        { int i; for (i=0; i<40; i++) printf("\b"); }

		ret = millet_read_source_yuv_file(gpec);
		if (ret <=0 )
			break;

		ret = millet_encoder_encode(gpec);
		printf("%4d: size = %6d", f, ret);

        if (ret < 0)
            break;
    }

	millet_encoder_encode_flush(gpec);

	endtime = clock();
	totaltime = (double)( (endtime - starttime)/(double)CLOCKS_PER_SEC );

	millet_show_mean_time(gpec, totaltime);
	

	millet_exit_files(gpec);
	millet_exit_encoder(gpec);

	return 0;
}
