/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
package jp.libamtrs;
import	android.content.Context;
import	android.widget.RelativeLayout;
import	android.view.View;
import	com.google.android.gms.ads.AdRequest;
import	com.google.android.gms.ads.AdSize;
import	com.google.android.gms.ads.AdView;
import	com.google.android.gms.ads.MobileAds;


public class AdMob
{
	private	long		mNativeHandle;
	private AdView		mAdView;

	static public AdView intialize(long _native, String _unitId, boolean _bottom)
	{
		AdMob	thiz	= new AdMob(_native, AmtrsActivity.instance, _unitId, _bottom);
		AmtrsActivity.instance.addView(thiz.mAdView);
		return	thiz.mAdView;
	}

	public AdMob(long _native, Context _context, String _unitId, boolean _bottom)
	{
		mNativeHandle	= _native;

		mAdView			= new AdView(_context);
		mAdView.setAdSize(AdSize.BANNER);
		mAdView.setAdUnitId(_unitId);

		RelativeLayout.LayoutParams	lp	= new RelativeLayout.LayoutParams(
				RelativeLayout.LayoutParams.MATCH_PARENT,
				RelativeLayout.LayoutParams.WRAP_CONTENT
		);
		lp.addRule(_bottom ? RelativeLayout.ALIGN_PARENT_BOTTOM : RelativeLayout.ALIGN_PARENT_TOP);
		mAdView.setLayoutParams(lp);

		AdRequest.Builder adRequestBuilder = new AdRequest.Builder();
		mAdView.loadAd(adRequestBuilder.build());
/*
		mAdView.addOnLayoutChangeListener(new View.OnLayoutChangeListener()
		{
			public void onLayoutChange(View v, int left, int top, int right, int bottom, int leftWas, int topWas, int rightWas, int bottomWas)
			{
				onSizeChange(mNativeHandle, left, top, right, bottom);
			}
		});
*/
	}

	public AdView view()
	{
		return	mAdView;
	}

	// AdMobのサイズが変わったことを通知する
	native private void onSizeChange(long _nativeThis, int left, int top, int right, int bottom);
}
