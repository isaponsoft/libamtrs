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
#ifndef	__libamtrs__window__display__hpp
#define	__libamtrs__window__display__hpp
#include "../graphics/rect.hpp"
#include "../graphics/size.hpp"
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

class	display
		: public ref_object
{
public:
	static ref<display> get_display(int _display_number = 0);

	//! ディスプレイのネイティブな pixel par inch を取得します。
	//! ただし、デスクトップOSなど解像度が存在しない場合は -1 を返します。
	virtual float pixel_par_inch() = 0;

	//! ディスプレイの論理サイズを取得します。
	//! iOSでは、ポイント単位になります。
	virtual size2<int> size() = 0;

	//! コンテンツを表示すべきサイズを取得します。
	virtual rect<int> content_rect()
	{
		vec2<int>	p	= {0, 0};
		size2<int>	s	= size();
		return	{p, s};
	}

	//! ポイントからピクセル数に変換するためのスケール値を取得します。
	virtual float pt_to_pixel_scale() = 0;
};



AMTRS_NAMESPACE_END

#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
#include "display-win32.hpp"
#endif

#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_ANDROID
#include "display-android.hpp"
#endif

#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
#include "display-ios.hpp"
#endif

#endif
