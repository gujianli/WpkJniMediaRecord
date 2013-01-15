// millet.h

#ifndef _millet_
#define _millet_ 1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINCE
#include "millet_types.h"
#else
#include <stdint.h>
#endif

#define MILLET_FASTEST_SETTING 1

#define MILLET_LOCAL  static
#define MILLET_GLOBAL extern

typedef enum _millet_rotate_mode{
	ML_ROTATE000=0,
	ML_ROTATE090=1,
	ML_ROTATE180=2,
	ML_ROTATE270=3,
} millet_rotate_mode;

typedef enum _millet_zoom_mode{
	ML_ZOOMOUT1X=0,
	ML_ZOOMOUT4X=1,
} millet_encode_mode;

typedef enum _millet_yuv_format{
	ML_YUV420SP = 0,//Y planar, VU interleave
	ML_YUV420P  = 1,//Y U V planar
} millet_yuv_format;

#define MILLET_MAX_FILE_NAME_SIZE 60

#define MILLET_SOURCE_YUV_FILE  1
#define MILLET_264_STREAM_FILE  1
#define MILLET_264_INFO_FILE    1
#define MILLET_DECODED_YUV_FILE 0




typedef struct _millet_encoder{
	int width;
	int height;
	int yuv_size;

	int fps_control_flag;
	int fps;

	int rate_control_flag;
	int bitrate;

	int gop_length;

	int rotate_mode;
	int zoom_mode;
	int yuv_format;
	int yuv_offset;

	uint8_t *f_yuv;
	uint8_t *g_yuv;
	int32_t *g_264;

	void* mcdull;

	int frame_counter;

#if MILLET_SOURCE_YUV_FILE
	char filename_f_yuv[MILLET_MAX_FILE_NAME_SIZE];
	FILE *file_f_yuv;
	int write_source_yuv_file_flag;
#endif//MILLET_SOURCE_YUV_FILE

#if	MILLET_264_STREAM_FILE
	char filename_g_264[MILLET_MAX_FILE_NAME_SIZE];
	FILE *file_g_264;
#endif//MILLET_264_STREAM_FILE

#if MILLET_264_INFO_FILE
	char filename_info[MILLET_MAX_FILE_NAME_SIZE];
	FILE *file_info;
#endif

#if MILLET_DECODED_YUV_FILE
	char filename_g_yuv[MILLET_MAX_FILE_NAME_SIZE];
	FILE *file_g_yuv;
#endif


}millet_encoder_t;



void millet_exit_encoder(void *p);

int millet_init_encoder(void *p, 
						int width, 
						int height,
						int fps,
						int bitrate,
						int gop_length);

void millet_init_preprocessor(void *p,
							 int rotate_mode,
							 int zoom_mode,
							 int yuv_format,
							 int yuv_offset);


void millet_exit_files(void *p);

int millet_init_files(void *p,
					  char *filename,
					  int write_source_yuv_file);

int  millet_read_source_yuv_file (void *p);
void millet_write_source_yuv_file(void *p);

int millet_encoder_encode      (void *p);
int millet_encoder_encode_flush(void *p);

void millet_show_mean_time(void *p, double total_time);


#ifdef __cplusplus
}
#endif

#endif // _millet_
