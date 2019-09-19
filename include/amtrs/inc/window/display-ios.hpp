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
