package jp.libamtrs.camera;
import	android.content.Context;
import	android.os.Bundle;
import	android.os.Handler;
import	android.os.Looper;
import	android.view.KeyEvent;
import	android.view.MotionEvent;
import	android.view.View;
import	android.view.ViewGroup;
import	android.widget.FrameLayout;


public class	Camera
				implements	android.hardware.Camera.PreviewCallback
{
	private	android.hardware.Camera			mCamera;
	private	long							mNativeHandle;
	private	android.graphics.SurfaceTexture	mBuffer;


	public Camera(long _nativeHandle, Context _context, android.hardware.Camera _camera)
	{
		mCamera			= _camera;
		mNativeHandle	= _nativeHandle;

		try
		{
			mBuffer		= new android.graphics.SurfaceTexture(0);
			mCamera.setPreviewTexture(mBuffer);

		} catch (java.io.IOException e) {}

		android.hardware.Camera.Parameters	parameters = mCamera.getParameters();
//		parameters.setPreviewSize(800, 600);
		parameters.setPreviewFormat(android.graphics.ImageFormat.YV12);
//		parameters.setPreviewFormat(android.graphics.ImageFormat.NV21);
		mCamera.setParameters(parameters);

		mCamera.setPreviewCallback(this);
		mCamera.startPreview();
	}


	public Camera(long _nativeHandle, Context _context, android.hardware.Camera _camera, int _width, int _height)
	{
		mCamera			= _camera;
		mNativeHandle	= _nativeHandle;

		android.hardware.Camera.Parameters	params = mCamera.getParameters();
		params.setPreviewSize(_width, _height);
		mCamera.setParameters(params);
		mCamera.setPreviewCallback(this);
mCamera.startPreview();
	}



	@Override
	public void onPreviewFrame(byte[] _data, android.hardware.Camera _camera)
	{
		onPreviewFrame(mNativeHandle, _data, _camera);
	}


	native private void onPreviewFrame(long _nativeHandle, byte[] _data, android.hardware.Camera _camera); 
}
