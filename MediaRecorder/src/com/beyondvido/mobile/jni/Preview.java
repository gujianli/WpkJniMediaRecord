package com.beyondvido.mobile.jni;

import java.io.IOException;
import java.util.List;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.view.SurfaceHolder;
import android.view.SurfaceView;


class Preview extends SurfaceView implements SurfaceHolder.Callback,Camera.PreviewCallback {
  SurfaceHolder mHolder;
  Camera mCamera;
  
  Preview(Context context) {
      super(context);

      // Install a SurfaceHolder.Callback so we get notified when the
      // underlying surface is created and destroyed.
      mHolder = getHolder();
      mHolder.addCallback(this);
      mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);        
  }   
  
  public void surfaceCreated(SurfaceHolder holder) {
      // The Surface has been created, acquire the camera and tell it where
      // to draw.
      mCamera = Camera.open();
      try
      {
         mCamera.setPreviewDisplay(holder);
         mCamera.setPreviewCallback(this);//added by zjl
      }
      catch (IOException exception)
      {
          mCamera.setPreviewCallback(null);//added by zjl
          mCamera.release();
          mCamera = null;           
          // TODO: add more exception handling logic here
      }
  }

  public void surfaceDestroyed(SurfaceHolder holder)
  {
      // Surface will be destroyed when we return, so stop the preview.
      // Because the CameraDevice object is not a shared resource, it's very
      // important to release it when the activity is paused.
      mCamera.stopPreview();
      mCamera.setPreviewCallback(null);//added by zjl
      mCamera.release();
      mCamera = null;
  }


  private Size getOptimalPreviewSize(List<Size> sizes, int w, int h)
  {
      final double ASPECT_TOLERANCE = 0.05;
      double targetRatio = (double) w / h;
      if (sizes == null) return null;

      Size optimalSize = null;
      double minDiff = Double.MAX_VALUE;

      int targetHeight = h;

      // Try to find an size match aspect ratio and size
      for (Size size : sizes) {
          double ratio = (double) size.width / size.height;
          if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE) continue;
          if (Math.abs(size.height - targetHeight) < minDiff) {
              optimalSize = size;
              minDiff = Math.abs(size.height - targetHeight);
          }
      }

      // Cannot find the one match the aspect ratio, ignore the requirement
      if (optimalSize == null) {
          minDiff = Double.MAX_VALUE;
          for (Size size : sizes) {
              if (Math.abs(size.height - targetHeight) < minDiff) {
                  optimalSize = size;
                  minDiff = Math.abs(size.height - targetHeight);
              }
          }
      }
      return optimalSize;
  }

  public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
      // Now that the size is known, set up the camera parameters and begin
      // the preview.
      Camera.Parameters parameters = mCamera.getParameters();

//      List<Size> sizes = parameters.getSupportedPreviewSizes();
//      Size optimalSize = getOptimalPreviewSize(sizes, w, h);
//      parameters.setPreviewSize(optimalSize.width, optimalSize.height);

     
      parameters.setPreviewSize(mPreviewSize.width, mPreviewSize.height);

      mCamera.setParameters(parameters);
      mCamera.startPreview();
  }

  
  
//below added by zjl    
  Size mPreviewSize;
  public void setPreviewSize(int width, int height)
  {
	  Camera cam = Camera.open();
	  Camera.Parameters parameters = cam.getParameters();
	  List<Size> sizes = parameters.getSupportedPreviewSizes();
      mPreviewSize = getOptimalPreviewSize(sizes, width, height);
      cam.release();
  }
  
  public Size getPreviewSize()
  {
      return mPreviewSize;
  }

  
  WpkJniMediaRecord mMediaRecorder;
  
  public void setMediaRecorder(WpkJniMediaRecord wjmr)
  {
	  mMediaRecorder = wjmr;
  }
  
  static int mFrameCounter=0;
  static boolean mIsFirstVideoFrame = true;
  public void onPreviewFrame(byte[] data, Camera camera)
  {

//  	if(mFrameCounter > 31) //mPreviewFps // ������һ�������
  	{
  	
  		// ����ƵӦͬʱ��ʼ
  	  if(mIsFirstVideoFrame)  	  
  	  {
  		  mIsFirstVideoFrame = false;
		  mMediaRecorder.audioEncodeStart();
  	  }
  	  
  	  //Note: apk writer should do some frame-rate control here.
  		//if( (mFrameCounter % 3)== 0)//��3ȡ1  		
  			mMediaRecorder.videoEncodeOneFrame(data);
  	} 
  	
  	mFrameCounter++;
  }    
}


