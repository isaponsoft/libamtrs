/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__window__display_ios__hpp
#define	__libamtrs__window__display_ios__hpp
#import <UIKit/UIKit.h>
#include "display.hpp"
AMTRS_NAMESPACE_BEGIN

// https://developer.apple.com/library/content/documentation/DeviceInformation/Reference/iOSDeviceCompatibility/Displays/Displays.html


class	ios_display : public display
{
public:
	ios_display()
	{
	}

	virtual float pixel_par_inch() override
	{
		return	mPPI;
	}


	virtual float pt_to_pixel_scale() override
	{
		return	mScale;
	}


	virtual size2<int> size() override
	{
		// iOS 8 以降ならこちらで見るべきかどうか要検討
		// UIScreen.mainScreen().nativeBounds

		// 毎回計算する。
		CGRect	ss		= [[UIScreen mainScreen] bounds];
		auto	width	= ss.size.width  * mScale;
		auto	height	= ss.size.height * mScale;
		return	size2<int>(width, height);
	}

	virtual rect<int> content_rect() override
	{
		if (@available(iOS 11.0, *))
		{
			// iOS 11 以降はノッチに対応しないとならない。
			//CGRect		ss				= [[UIScreen mainScreen] bounds];
			UIWindow*	window			= UIApplication.sharedApplication.keyWindow;
			auto		displaySize		= size();
			#if 0
			AMTRS_DEBUG_LOG("size = %dx%d  content rec = %d %d %d %d", 
				displaySize.width,
				displaySize.height,
                (int)window.safeAreaInsets.left,
				(int)window.safeAreaInsets.top,
				(int)window.safeAreaInsets.right,
				(int)window.safeAreaInsets.bottom)
			#endif
			return	{
                (int)(window.safeAreaInsets.left * mScale),
				(int)(window.safeAreaInsets.top  * mScale),
				(int)displaySize.width  - (int)((window.safeAreaInsets.right  + window.safeAreaInsets.left) * mScale),
				(int)displaySize.height - (int)((window.safeAreaInsets.bottom + window.safeAreaInsets.top ) * mScale)
			};
		}
		else
		{
			return	display::content_rect();
		}
	}

private:
	void initialize()
	{
		// スケールの取得
		if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
		{
			mScale	= [[UIScreen mainScreen] scale];
		}

		// PPIの計算
		if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
		{
			// iPad系は 132 base
			mPPI    = 132.0f * mScale;
		}
		else
		{
			// iPhone系やそれ以外のデバイスは 160 base
			mPPI    = 160.0f * mScale;
		}		
	}

	float		mScale	= 1.0f;
	float		mPPI;

	friend	display;
};


inline ref<display> display::get_display(int _display_number)
{
	if (_display_number != 0)
	{
		throw	std::exception();
	}
	ref<ios_display>	thiz	= new ios_display();
	thiz->initialize();
	return	thiz;
}


AMTRS_NAMESPACE_END
#endif
