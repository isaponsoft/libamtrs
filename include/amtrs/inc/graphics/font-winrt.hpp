#ifndef	__libamtrs__bitmaps__font_winrt__hpp
#define	__libamtrs__bitmaps__font_winrt__hpp
#include <atomic>
#include <iostream>
#include "../../string.hpp"
#include "../iostream/rstream.hpp"
#include "../../os/win32/def.hpp"
#include "../../os/win32/error_code.hpp"
#pragma comment(lib, "gdiplus.lib")
AMTRS_NAMESPACE_BEGIN

//! このクラスはプラットフォームの機能を利用します。
class	win32_font
		: public font
{
public:
	template<class... Args>
	win32_font(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
		: font(_fontname, _default_size, _default_color)
	{
	}

	~win32_font()
	{
	}


	void on_update()
	{
	}


	virtual void on_draw(bitmap<pixel_type>& _out, vec2<int> _origin, size2<int>* _clip, std::string_view _text, const draw_info& _info) override
	{
	}


	virtual auto get_metrics() -> metrics override
	{
		metrics	retval;
		return	retval;
	}


	virtual bounds compute_bounds(std::string_view _text) override
	{
		bounds	retval;
		return	retval;
	}

	virtual auto compute_braek(std::string_view _text, int _maxWidth) -> std::pair<typename std::string_view::size_type, int> override
	{
		return	{};
	}



	void release()
	{
	}

	std::string	mFontname;
	HFONT		mFont		= nullptr;
	HFONT		mPrevious	= nullptr;
	HBRUSH		mClearBrush	= nullptr;
	HDC			mDC			= nullptr;
	LOGFONTA	mLogFont;


	friend class amtrs::font;
};



inline ref<font> font::create(pixel_type _default_color)
{
	return	create(0, _default_color);
}


inline ref<font> font::create(font_size_type _default_size, pixel_type _default_color)
{
	ref<win32_font>	f	= new win32_font(nullptr, _default_size, _default_color);
	return	f;
}


inline ref<font> font::create(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
{
	ref<win32_font>	f	= new win32_font(_fontname, _default_size, _default_color);
	return	f;
}

inline auto font::system_font_size() -> font_size_type
{
	return	static_cast<font_size_type>(0);
}





AMTRS_NAMESPACE_END
#endif
