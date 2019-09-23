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
#ifndef	__libamtrs__window__display_android__hpp
#define	__libamtrs__window__display_android__hpp
#include "../os/android/android_app.hpp"
#include "../java/android/content/Context.hpp"
#include "../java/android/content/res/Resources.hpp"
#include "../java/android/util/DisplayMetrics.hpp"
#include "../java/android/view/Display.hpp"
#include "../java/android/view/WindowManager.hpp"
#include "display.hpp"
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
		auto	display		= os::android::get_activity().getWindowManager().getDefaultDisplay();
		return	size2<int>(
					display.getWidth(),
					display.getHeight()
				);
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
