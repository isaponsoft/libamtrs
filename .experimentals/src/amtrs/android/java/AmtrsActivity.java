/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
package jp.libamtrs;
import	android.content.Intent;
import	android.os.Bundle;
import	android.os.Handler;
import	android.os.Looper;
import	android.view.KeyEvent;
import	android.view.MotionEvent;
import	android.view.View;
import	android.view.ViewGroup;
import	android.view.WindowManager;
import	android.widget.FrameLayout;
import	android.widget.RelativeLayout;
import	android.widget.LinearLayout;

import	android.os.Handler;
import	android.os.Looper;
import	java.lang.Runnable;

import	java.util.HashMap;


public class AmtrsActivity extends androidx.fragment.app.FragmentActivity 
{
	public	RelativeLayout	  							mContentsFrame;
	public	FrameLayout									mFrame;
	private	long										mNativeHandle;
	private	Handler										mLoopHandler;
	private	Runnable									mUpdater;
	private	boolean										mIsForeground;
	public static AmtrsActivity							instance;

	@Override
	protected void onCreate(Bundle _savedInstanceState)
	{
		super.onCreate(_savedInstanceState);
		instance	= this;
		ActivityInitializer.activity	= this;
		mNativeHandle	= ActivityInitializer.initialize(this);
		mContentsFrame	= new RelativeLayout(this);
		mContentsFrame.setLayoutParams(new RelativeLayout.LayoutParams(
				FrameLayout.LayoutParams.MATCH_PARENT,
				FrameLayout.LayoutParams.MATCH_PARENT
		));
		setContentView(mContentsFrame);


		// コンテンツを格納するフレーム
		mContentsFrame.addView(mFrame = new FrameLayout(this), new FrameLayout.LayoutParams(
				LinearLayout.LayoutParams.MATCH_PARENT,
				LinearLayout.LayoutParams.WRAP_CONTENT
		));


		//getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		onCreate(mNativeHandle);


		mLoopHandler	= new Handler(Looper.getMainLooper());
		mUpdater		= new Runnable()
		{
			private	int	loopCount	= 0;

			@Override
			public void run()
			{
				long	thiz	= mNativeHandle;
				if (thiz != 0)
				{
					onLooper(thiz);
					mLoopHandler.post(this);
				}
			//	java.lang.Runtime.getRuntime().gc();
			}
		};
		mLoopHandler.post(mUpdater);
	}


	public void addView(View _view)
	{
		mContentsFrame.addView(_view);
	}


	@Override
	protected void onActivityResult(int _requestCode, int _resultCode, Intent _data)
	{
		super.onActivityResult(_requestCode, _resultCode, _data);
		onActivityResult(mNativeHandle, _requestCode, _resultCode, _data);
	}


	public void foreground()
	{
		if (!mIsForeground)
		{
			startActivity(new Intent(getApplicationContext(), ActivityInitializer.activity.getClass()));
		}
	}


	public void pushView(View _view)
	{
		mFrame.addView(_view);
		_view.setFocusable(true);
		_view.setFocusableInTouchMode(true);
		_view.requestFocus();
	}


	public void popView()
	{
		Handler	handler	= new Handler();
		handler.post(new Runnable() {
			@Override
			public void run() {
				int	last	= mContentsFrame.getChildCount() - 1;
				if (last >= 0)
				{
					mContentsFrame.removeViewAt(last);
				}
			}
		});
	}


	@Override
	public void onStart()
	{
		super.onStart();
		onStart(mNativeHandle);
	}


	@Override
	public void onRestart()
	{
		super.onPause();
		onRestart(mNativeHandle);
	}


	@Override
	public void onResume()
	{
		super.onResume();

		// 画面の回転を行っている場合に、これがないとサーフェースの再作成が
		// 行われないことがる
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
		onResume(mNativeHandle);
		mIsForeground	= true;
	}


	@Override
	public void onPause()
	{
		onPause(mNativeHandle);
		super.onPause();
	}


	@Override
	public void onStop()
	{
		mIsForeground	= false;
		onStop(mNativeHandle);
		super.onStop();
	}


	@Override
	public void onDestroy()
	{
		onDestroy(mNativeHandle);
		mLoopHandler.removeCallbacks(mUpdater);
		mLoopHandler	= null;
		mNativeHandle	= 0;
		super.onDestroy();
	}


	@Override
	public void onRequestPermissionsResult(int _requestCode, String[] _permissions, int[] _grantResults)
	{
		onRequestPermissionsResult(mNativeHandle, _requestCode, _permissions, _grantResults);
	}


	private native void onCreate(long _nativeThis);
	private native void onRestart(long _nativeThis);
	private native void onStart(long _nativeThis);
	private native void onResume(long _nativeThis);
	private native void onPause(long _nativeThis);
	private native void onStop(long _nativeThis);
	private native void onDestroy(long _nativeThis);
	private native void onLooper(long _nativeThis);
	private native void onRequestPermissionsResult(long _nativeThis, int _requestCode, String[] _permissions, int[] _grantResults);
	private native void onActivityResult(long _nativeThis, int _requestCode, int _resultCode, Intent _data);
}
