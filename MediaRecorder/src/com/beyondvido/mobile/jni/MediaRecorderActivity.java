package com.beyondvido.mobile.jni;

import android.app.Activity;
import android.os.Bundle;
/*
import android.hardware.Camera.Size;
import android.view.Window;


public class MediaRecorderActivity extends Activity {
   
    private Preview mPreview;    
    private WpkJniMediaRecord mMediaRecorder;    
	
    @Override
	protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Hide the window title.
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        // Create our Preview view and set it as the content of our activity.        
        mPreview = new Preview(this);        
        
        int targetWidth = 320;
        int targetHeight = 240;         
        mPreview.setPreviewSize(targetWidth, targetHeight);
        Size videoSize = mPreview.getPreviewSize();
        
        int videoFrameRate = 12;
        int videoBitrate = 100;
        int videoGopLength = videoFrameRate*10;
        
        mMediaRecorder = new WpkJniMediaRecord(
        		videoSize.width,
        		videoSize.height,
        		videoFrameRate,
        		videoBitrate,
        		videoGopLength,
        		//以下3个参数根据不同手机调整
        		0,//ROTATE000
        		0,//ZOOMOUT1X
        		0,//YUV420SP
        		16);
        
        mPreview.setMediaRecorder(mMediaRecorder);//for preview method: onPreviewFrame        
        
        setContentView(mPreview);        
    }
    
    protected void onDestroy() {
    	super.onDestroy();    	
    	
    	mMediaRecorder.release();
    } 
}*/


import android.widget.TextView;
public class MediaRecorderActivity extends Activity {
    
    
    // Called when the activity is first created. 
    @Override
    
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        TextView tv = new TextView(this);
        
        mMediaRecorder = new WpkJniMediaRecord();
        //MilletYuvTo264();
        //tv.setText("Hello,World!");
        tv.setText(mMediaRecorder.MilletYuvTo264());
        setContentView(tv);
     }
    public WpkJniMediaRecord mMediaRecorder;  
 
}



