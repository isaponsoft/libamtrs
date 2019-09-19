#ifndef	__libamtrs__os__win32__gdi_font__hpp
#define	__libamtrs__os__win32__gdi_font__hpp
#include <string_view>
#include "windows.hpp"
#include "def.hpp"
#include "gdi_dc.hpp"
AMTRS_OS_WIN32_NAMESPACE_BEGIN


struct	gdi_font
{
	using	height_type		= LONG;
	using	logfont_type	= LOGFONTW;

	//! フォント名と高さを指定してフォントを作成します。
	static HFONT create(std::wstring_view _facename, LONG _height)
	{
		logfont_type		lf = {};
		lf.lfHeight			= _height;
		lf.lfCharSet		= DEFAULT_CHARSET;
		lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
		lf.lfQuality		= DEFAULT_QUALITY;
		lf.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
		for (unsigned int i = 0; i < _facename.size(); ++i)
		{
			lf.lfFaceName[i] = _facename[i];
		}
		return	CreateFontIndirectW(&lf);
	}

	//! フォントの情報を取得します。
	//! 第２引数を省略した場合は内部でデバイスコンテキストを用意します。
	static TEXTMETRIC get_metrix(HDC _dc, HFONT _font)
	{
		TEXTMETRIC		retval;
		HGDIOBJ			old		= SelectObject(_dc, _font);
		GetTextMetrics(_dc, &retval);
		SelectObject(_dc, old);
		return	retval;
	}

	//! フォントの情報を取得します。
	//! 第２引数を省略した場合は内部でデバイスコンテキストを用意します。
	static TEXTMETRIC get_metrix(HFONT _font)
	{
		compatible_dc	compati(nullptr);
		return	get_metrix(compati.get(), _font);
	}

};


AMTRS_OS_WIN32_NAMESPACE_END
#endif
