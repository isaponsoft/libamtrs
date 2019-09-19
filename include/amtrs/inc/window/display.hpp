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
