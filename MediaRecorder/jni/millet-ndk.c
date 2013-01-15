//
// millet-ndk.c
//
#include <string.h>
#include <jni.h>
/*
extern int millet_write_preview_info(int fps, int yuv_interval);

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletWritePreviewInfo( JNIEnv* env,
											jobject thiz,
											int fps, int yuv_interval)
{
	millet_write_preview_info(fps, yuv_interval);
}

int millet_init_encoder(int width, int height, int fps, int bitrate, int gop_length,
		int rotate_mode, int zoom_mode, int yuv_format, int yuv_offset);
//extern int millet_init_encoder(int *param);
extern void millet_exit_encoder(int flush_flag);
extern int millet_encode_frame(unsigned char *yuv420sp);
extern int millet_write_input_yuv(unsigned char *yuv420sp);

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletInit( JNIEnv* env,
											jobject thiz,
											int width, int height,
											int fps, int bitrate, int gop_length,
											int rotate_mode, int zoom_mode, int yuv_format, int yuv_offset)
{
//	millet_init_encoder(param);
	millet_init_encoder(width, height, fps, bitrate, gop_length,
			rotate_mode, zoom_mode, yuv_format, yuv_offset);

}

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletExit( JNIEnv* env,
											jobject thiz )
{
	millet_exit_encoder(1);
}

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletEncode( JNIEnv* env,
											jobject thiz, unsigned char *yuv420sp)
{
	millet_encode_frame(yuv420sp);
	//	millet_write_input_yuv(yuv420sp);
}

*/

extern int main_millet(int argc, char* argv[]);

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletYuvTo264( JNIEnv* env,
											jobject thiz )
{
	main_millet(1, NULL);
    return (*env)->NewStringUTF(env, "MILLET YUV TO 264 DONE!");
}

/*
int millet_set_avc_file_name(char *avc_file_name);

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletSetAvcFileName( JNIEnv* env,
											jobject thiz, jstring avc_file_name )
{
	 millet_set_avc_file_name( (char*)(*env)->GetStringUTFChars(env,avc_file_name,0)  );
}


extern int main_av_files(int argc, char** argv, double VideoFrameRate);

jstring
Java_com_beyondvido_mobile_jni_WpkJniMediaRecord_MilletAmrAvcFilesToMp4( JNIEnv* env,
											jobject thiz,
											int avc_frame_rate, jstring avc_file_name,
											jstring amr_file_name,
											jstring mp4_file_name)
{
	 char *amr_name =  (char*)(*env)->GetStringUTFChars(env, amr_file_name, 0);
	 char *avc_name =  (char*)(*env)->GetStringUTFChars(env, avc_file_name, 0);
	 char *mp4_name =  (char*)(*env)->GetStringUTFChars(env, mp4_file_name, 0);


	char *argv[4] = {
			"MilletMp4Muxer",
			amr_name,
			avc_name,
			mp4_name
	};

	main_av_files(4, argv, avc_frame_rate);
    //return (*env)->NewStringUTF(env, "MCDULL MP4 Muxer DONE!");
}
*/


