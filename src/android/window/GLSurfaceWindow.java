package jp.libamtrs.window;
import android.content.Context;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLSurfaceWindow extends GLSurfaceView implements GLSurfaceView.Renderer
{
	private boolean	mDrawReady	= false;

	public GLSurfaceWindow(Context _context, long _nativeHandle)
	{
		super(_context);

		mNativeHandle	= _nativeHandle;
		setEGLContextClientVersion(2);		// OepnGL ES 2.0
		setEGLConfigChooser(8, 8, 8, 8, 16, 8);
		setRenderer(this);
		setRenderMode(RENDERMODE_WHEN_DIRTY);
	}



	@Override
	public void onDrawFrame(GL10 gl)
	{
		if (mNativeHandle != 0)
		{
			onDrawFrame(mNativeHandle);
		}
	}


	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		onSurfaceChanged(mNativeHandle, width, height);
	}


	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		onSurfaceCreated(mNativeHandle, config);
	}


	native private void onDrawFrame(long _nativeHandle);
	native private void onSurfaceChanged(long _nativeHandle, int width, int height);
	native private void onSurfaceCreated(long _nativeHandle, EGLConfig config);


	// ************************************************************************
	// window.template.txt
	// ************************************************************************
	
	private	long	mNativeHandle;
	
	
	@Override
	public boolean onTouchEvent(MotionEvent _motionEvent)
	{
		if (mNativeHandle != 0)
		{
			return	onTouchEvent(mNativeHandle, _motionEvent);
		}
		return	super.onTouchEvent(_motionEvent);
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent _event)
	{
		if (mNativeHandle != 0)
		{
			if (dispatchKeyEvent(mNativeHandle, _event))
			{
				return	true;
			}
		}
		return	super.dispatchKeyEvent(_event);
	}
	
	native private boolean onTouchEvent(long _nativeHandle, MotionEvent _motionEvent);

	native private boolean dispatchKeyEvent(long _nativeHandle, KeyEvent _event);
}
