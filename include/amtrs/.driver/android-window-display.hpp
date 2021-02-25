/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__window__display_android__hpp
#define	__libamtrs__window__display_android__hpp
#include "android-api-android_app.hpp"
#include "../java/android/content/Context.hpp"
#include "../java/android/content/res/Resources.hpp"
#include "../java/android/util/DisplayMetrics.hpp"
#include "../java/android/view/Display.hpp"
#include "../java/android/view/WindowManager.hpp"
AMTRS_NAMESPACE_BEGIN

class	display_android : public display
{
	struct	display_metrics
	{
		float	density;
		float	densityDpi;
		float	scaledDensity;
	};

public:
	display_android()
	{
		auto	dm			= os::android::get_activity().getResources().getDisplayMetrics();
		mDM.density			= dm.get_density();
		mDM.densityDpi		= dm.get_densityDpi();
		mDM.scaledDensity	= dm.get_scaledDensity();
	//	AMTRS_DEBUG_LOG("density =%f, density DPI = %f, scaledDensity = %f", mDM.density, mDM.densityDpi, mDM.scaledDensity);
	}

	virtual float pixel_par_inch() override
	{
		return	density_dpi();
	}

	virtual float pt_to_pixel_scale() override
	{
		return	mDM.scaledDensity;
	}


	virtual size2<int> size() override
	{
		auto	wm			= os::android::get_activity().getWindowManager();
		auto	display		= wm.getDefaultDisplay();
		return	size2<int>(
					display.getWidth(),
					display.getHeight()
				);
	}

	//! ウィンドウモードが有効か問い合わせる
	virtual bool is_window_mode_enable() override
	{
		return	false;
	}



	float density() const noexcept
	{
		return	mDM.density;
	}

	float density_dpi() const noexcept
	{
		return	mDM.densityDpi;
	}


private:
	display_metrics	mDM;

	friend	display;
};


inline ref<display> display::get_display(int _display_number)
{
	if (_display_number != 0)
	{
		throw	std::exception();
	}
	ref<display_android>	thiz = new display_android();
	return	thiz;
}


AMTRS_NAMESPACE_END
#endif
