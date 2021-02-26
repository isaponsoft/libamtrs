/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
package jp.libamtrs.window;
import android.content.Context;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class SurfaceView
				extends		android.view.SurfaceView
				implements	android.view.SurfaceHolder.Callback
{
	public SurfaceView(Context _context, long _nativeHandle)
	{
		super(_context);
		mNativeHandle	= _nativeHandle;
		getHolder().addCallback(this);
	}


	@Override
	public void surfaceChanged(android.view.SurfaceHolder holder, int format, int width, int height)
	{
		surfaceChanged(mNativeHandle, holder, format, width, height);
	}

	@Override
	public void surfaceCreated(android.view.SurfaceHolder holder)
	{
		surfaceCreated(mNativeHandle, holder);
	}

	@Override
	public void surfaceDestroyed(android.view.SurfaceHolder holder)
	{
		surfaceDestroyed(mNativeHandle, holder);
	}


	native private void surfaceChanged(long _nativeHandle, android.view.SurfaceHolder holder, int format, int width, int height);
	native private void surfaceCreated(long _nativeHandle, android.view.SurfaceHolder holder);
	native private void surfaceDestroyed(long _nativeHandle, android.view.SurfaceHolder holder);


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
