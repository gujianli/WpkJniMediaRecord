package com.beyondvido.mobile.jni;



import android.media.MediaRecorder;  
import java.io.File;
import java.io.IOException; 

public class WpkJniMediaRecord {
	
	// members for audio
	private MediaRecorder mAmrRecorder;	
	private File mAmrFile = null; 
    
    // members for video
	int mVideoWidth, mVideoHeight, mVideoFrameRate, mVideoBitrate, mVideoGopLength;
	int mVideoRotateMode, mVideoZoomMode, mYuvFormat, mYuvOffset;
	String mPath, mAmrFileName, mAvcFileName, mMp4FileName;
	 
	public WpkJniMediaRecord() {}
	
	public WpkJniMediaRecord(int mVideoWidth, int mVideoHeight,
			int mVideoFrameRate, int mVideoBitrate, int mVideoGopLength,
			int mVideoRotateMode, int mVideoZoomMode, int mYuvFormat,
			int mYuvOffset) {
		super();
		
		// file name init
		mPath = "/sdcard/android/jzeng.";
		mAmrFileName = mPath + "amr";
		mAvcFileName = mPath + "264";
		mMp4FileName = mPath + "mp4";
		
		// video init
		this.mVideoWidth = mVideoWidth;
		this.mVideoHeight = mVideoHeight;
		this.mVideoFrameRate = mVideoFrameRate;
		this.mVideoBitrate = mVideoBitrate;
		this.mVideoGopLength = mVideoGopLength;
		this.mVideoRotateMode = mVideoRotateMode;
		this.mVideoZoomMode = mVideoZoomMode;
		this.mYuvFormat = mYuvFormat;
		this.mYuvOffset = mYuvOffset;
		
		MilletInit(mVideoWidth, mVideoHeight, mVideoFrameRate, mVideoBitrate, mVideoGopLength,
				mVideoRotateMode, mVideoZoomMode, mYuvFormat, mYuvOffset);
		MilletSetAvcFileName( mAvcFileName );// must be call after MilletInit().

		
		// audio init
		 try {   
             // ����¼����ʱ�ļ�   
             mAmrRecorder = new MediaRecorder();  
             mAmrFile =  new File(mAmrFileName);
                     
     		mAmrRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);// ����������Դ����˷� 
    		mAmrRecorder.setOutputFormat(MediaRecorder.OutputFormat.RAW_AMR);// ���ø�ʽ                    
    		mAmrRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);// ���ñ����� 
    		mAmrRecorder.setOutputFile(mAmrFile.getAbsolutePath());// ��������ļ�·��   
    		mAmrRecorder.prepare();
             
         } catch (IOException e) {   
             // TODO Auto-generated catch block   
             e.printStackTrace();   
         } 
	}
	
	
	public void release()
	{
		// video exit
		MilletExit();		
		
		//audio exit
		mAmrRecorder.stop();   
		mAmrRecorder.release();   
		mAmrRecorder = null; 
		
		MilletAmrAvcFilesToMp4(mVideoFrameRate, mAvcFileName, mAmrFileName, mMp4FileName);
	}
	
	public void videoEncodeOneFrame(byte[] data)
	{
		MilletEncode(data);
	}
	
	public void audioEncodeStart()
	{
		mAmrRecorder.start();
	}	
	
	
	
	
	// rotate_mode��  0 -->û����ת	
	//               1 -->��ת   90��
	//               2 -->��ת 180��
	//               3 -->��ת270��    
	//
	// yuv_format��  0 -->YUV420SP (Y planar, VU interleave)
	//              1 -->YUV420P (Y U V planar)
	//
	// zoom_mode��  0 -->��������	
	//             1 -->YUV��С4��
	//	
	// yuv_offset: ʵ��yuv������MilletEncode()������data�е�ƫ��
	private native String  MilletInit(int width, int height, int fps, int bitrate, int gop_length,
    		int rotate_mode, int zoom_mode, int yuv_format, int yuv_offset);

	// ���������.264�ļ���
	private native String  MilletSetAvcFileName( String avc_file_name );
	
	private native String  MilletExit();
	
	// ÿ����һ֡����һ��
	private native String  MilletEncode(byte[] yuv420);
	
	// ¼�ƽ���ǰ����.amr��.264�ļ��ϳ�.mp4�ļ�
	private native String  MilletAmrAvcFilesToMp4( 
			int avc_frame_rate, String avc_file_name,
			String amr_file_name,
			String mp4_file_name );
	    
	public native String  MilletYuvTo264();
	
	static
	{
		System.loadLibrary("millet-ndk" );
	}  
}
