/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bitmaps__font_win32__hpp
#define	__libamtrs__bitmaps__font_win32__hpp
#include "../string.hpp"
#include "win32-api-windows.hpp"
#include "win32-api-gdi_bitmap.hpp"
#include "win32-api-gdi_font.hpp"
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
		if (_fontname)
		{
			mFontname	= _fontname;
		}
	}

	~win32_font()
	{
		release();
		if (mClearBrush)
		{
			DeleteObject(mClearBrush);
		}
		if (mDC)
		{
			DeleteDC(mDC);
		}
	}


	void on_update()
	{
		if (!mDC)
		{
			mDC			= CreateCompatibleDC(nullptr);
			mClearBrush	= CreateSolidBrush(RGB(0, 0, 255));
		}
		release();
		std::memset(&mLogFont, 0, sizeof(mLogFont));
		mLogFont.lfHeight			= -mFontSize;			// 負の値にすると、絶対値指定扱いになる
		mLogFont.lfCharSet			= DEFAULT_CHARSET;
		mLogFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
		mLogFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
		mLogFont.lfQuality			= ANTIALIASED_QUALITY;//DEFAULT_QUALITY;		// ANTIALIASED_QUALITY
		mLogFont.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
		::strcpy_s(mLogFont.lfFaceName, mFontname.data());
		mFont	= CreateFontIndirectA(&mLogFont);
		if (!mFont)
		{
			throw	os::win32::make_system_error("create font error.");
		}
		mPrevious	= (HFONT)SelectObject(mDC, mFont);
	}


	virtual void on_draw(bitmap<pixel_type>& _out, vec2<int> _origin, size2<int>* _clip, std::string_view _text, const draw_info& _info) override
	{
		on_update();

		const auto		col			= _info.color;
		const auto		edg			= _info.edgeColor;
		const int		penWide		= (_info.edgeSize > 0) ? _info.edgeSize * 2 : 0;	// 枠線はエッジの中央に対して描画されるので、２倍のサイズで指定する
		const MAT2		mat			= {{0,1},{0,0},{0,0},{0,1}};						// GetGlyphOutline用

		// Unicode文字を取得
		auto			wstr		= u8_to_wstring(_text);

		// ビットマップを生成する
		BITMAPINFO bmpInfo = {};
		bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth		= _out.size().width ;
		bmpInfo.bmiHeader.biHeight		= _out.size().height;
		bmpInfo.bmiHeader.biPlanes		= 1;
		bmpInfo.bmiHeader.biBitCount	= 32;
		RGBQUAD*		pixels	= nullptr;
		HBITMAP			hBitMap	= CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&pixels, 0, 0);
		HBITMAP			oldBMP	= (HBITMAP)SelectObject(mDC, hBitMap);

		// BMP背景をクリア用のブラシで初期化
		{
			RECT 	bmpRect	= {0,0,bmpInfo.bmiHeader.biWidth,bmpInfo.bmiHeader.biHeight};
			FillRect(mDC, &bmpRect, mClearBrush);
		}

		// 背景色なし
		SetBkMode(mDC, TRANSPARENT);
		SetTextAlign(mDC, TA_BASELINE);

		if (penWide > 0)
		{
			HPEN   hPen			= (HPEN)CreatePen(PS_SOLID, penWide, RGB(0, 255, 0));
			HBRUSH hBrush		= (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
			HPEN   oldPen		= (HPEN)SelectObject(mDC, hPen);
			HBRUSH oldBrush		= (HBRUSH)SelectObject(mDC, hBrush);

			BeginPath(mDC);
			TextOutW(mDC, _origin.x, _origin.y, wstr.c_str(), (int)wstr.size());
			EndPath(mDC);
			StrokeAndFillPath(mDC);
			SelectObject(mDC, oldPen);
			SelectObject(mDC, oldBrush);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}

		// 文字を描画
		SetTextColor(mDC, RGB(255, 0, 0));
		TextOutW(mDC, _origin.x, _origin.y, wstr.c_str(), (int)wstr.size());


		if (penWide > 0)
		{
			// まずは枠を出力
			auto	in	= pixels;
			for (auto& c : _out.subimg({{0, 0}, _out.size()}).flip_vertical())
			{
				int	e	= in->rgbGreen;
				if (e > 0)
				{
					c.r	= (int)edg.r * e / 255;
					c.g	= (int)edg.g * e / 255;
					c.b	= (int)edg.b * e / 255;
					c.a	= (int)edg.a * e / 255;
				}
				++in;
			}
		}

		// 枠以外を出力
		auto	in	= pixels;
		for (auto& c : _out.subimg({{0, 0}, _out.size()}).flip_vertical())
		{
			int	a	= in->rgbRed;
			if (a > 0)
			{
				c.r	= ((int)col.r * a + (int)c.r * (255 - a)) / 255;
				c.g	= ((int)col.g * a + (int)c.g * (255 - a)) / 255;
				c.b	= ((int)col.b * a + (int)c.b * (255 - a)) / 255;
				c.a	= std::min((int)col.a * a / 255 + (int)c.a, 255);
			}
			++in;
		}

		SelectObject(mDC, oldBMP);
		DeleteObject(hBitMap);
	}


	virtual auto get_metrics() -> metrics override
	{
		on_update();

		TEXTMETRICA tm;
		GetTextMetrics(mDC, &tm);

		metrics	retval;
		retval.ascent	= tm.tmAscent;
		retval.descent	= tm.tmDescent;
		return	retval;
	}


	virtual bounds compute_bounds(std::string_view _text) override
	{
		on_update();

		TEXTMETRICA tm;
		GetTextMetrics(mDC, &tm);
		const MAT2		mat		= {{0,1}, {0,0}, {0,0}, {0,1}};
		GLYPHMETRICS	gm;

		int				minY	= tm.tmAscent;
		int				maxY	= 0;
		int				minX	= 0;
		int				x		= 0;
		int				right	= 0;
		bool			first	= true;
		auto*			cur		= _text.data();
		auto*			end		= cur + _text.size();
		while (cur < end)
		{
			int32_t	c;
			auto	nextCursor	= u8string_read_char(c, cur);
			if (!nextCursor)
			{
				++cur;
				continue;
			}
			cur	= nextCursor;

			GetGlyphOutlineW(mDC, c, GGO_BITMAP, &gm, 0, 0, &mat);
			if (first)
			{
				minX	= gm.gmptGlyphOrigin.x;
				minY	= -gm.gmptGlyphOrigin.y;
				first	= false;
			}
			if (minY > -gm.gmptGlyphOrigin.y)
			{
				minY	= -gm.gmptGlyphOrigin.y;
			}
			int		b	= gm.gmBlackBoxY - gm.gmptGlyphOrigin.y;
			if (maxY < b)
			{
				maxY	= b;
			}
			right	=  x + gm.gmptGlyphOrigin.x + gm.gmBlackBoxX;
			x		+= gm.gmCellIncX;
		}

		bounds	retval;
		retval.area		= {minX, minY,  right - minX, maxY - minY};
		retval.advance	= x;
		return	retval;
	}

	virtual auto compute_braek(std::string_view _text, int _maxWidth) -> std::pair<typename std::string_view::size_type, int> override
	{
		on_update();

		TEXTMETRICA tm;
		GetTextMetrics(mDC, &tm);
		const MAT2		mat		= {{0,1}, {0,0}, {0,0}, {0,1}};
		GLYPHMETRICS	gm;

		int				x		= 0;
		int				width	= 0;
		auto*			cur		= _text.data();
		auto*			end		= cur + _text.size();
		while (cur < end)
		{
			int32_t	c;
			auto	nextCursor	= u8string_read_char(c, cur);
			if (!nextCursor)
			{
				++cur;
				continue;
			}

			// グリフを取得
			GetGlyphOutlineW(mDC, static_cast<wchar_t>(c), GGO_BITMAP, &gm, 0, 0, &mat);

			auto	right	=  x + gm.gmptGlyphOrigin.x + gm.gmBlackBoxX;
			if (right > (unsigned int)_maxWidth)
			{
				break;
			}
			x		+= gm.gmCellIncX;
			width	=  right;
			cur		= nextCursor;
		}
		return	{cur - _text.data(), width};
	}


#if 0
	virtual auto on_create_image(bitmap<pixel_type>& _buffer, const std::string& _text) -> metrics
	{
		const int		penSize		= mEdgeSize * 2;
		auto			tmp			= u8_to_wstring({_text.c_str(), _text.size()});


		// 描画後のサイズを計算する
		RECT 	txtRect	= {0, 0, 0, 0};
		DrawTextW(mDC, tmp.c_str(), -1, &txtRect, DT_CALCRECT);

		RECT 	bmpRect	= txtRect;
		bmpRect.right	+= penSize;		// penSize / 2 + penSize / 2
		bmpRect.bottom	+= penSize;		// penSize / 2 + penSize / 2

		// ビットマップを生成する
		BITMAPINFO bmpInfo = {};
		bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth		= bmpRect.right - bmpRect.left;
		bmpInfo.bmiHeader.biHeight		= (bmpRect.bottom - bmpRect.top);
		bmpInfo.bmiHeader.biPlanes		= 1;
		bmpInfo.bmiHeader.biBitCount	= 32;
		unsigned char*	p = 0;
		HBITMAP	hBitMap	= CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&p, 0, 0);
		HBITMAP	oldBMP	= (HBITMAP)SelectObject(mDC, hBitMap);

		// BMP背景をクリア用のブラシで初期化
		FillRect(mDC, &bmpRect, mClearBrush);

		// 文字列を描画
		RECT 	drwRect	= txtRect;
		drwRect.left	= mEdgeSize;
		drwRect.top		= mEdgeSize;
		drwRect.right	+= mEdgeSize * 2;
		drwRect.bottom	+= mEdgeSize * 2;
		SetBkMode(mDC, TRANSPARENT);
		if (penSize > 0)
		{
			HPEN   hPen			= (HPEN)CreatePen(PS_SOLID, penSize, RGB(0, 255, 0));
			HBRUSH hBrush		= (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
			HPEN   oldPen		= (HPEN)SelectObject(mDC, hPen);
			HBRUSH oldBrush		= (HBRUSH)SelectObject(mDC, hBrush);

			BeginPath(mDC);
			DrawTextW(mDC, tmp.c_str(), -1, &drwRect, 0L);
			EndPath(mDC);
			StrokeAndFillPath(mDC);
			SelectObject(mDC, oldPen);
			SelectObject(mDC, oldBrush);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
		SetTextColor(mDC, RGB(255, 0, 0));
		DrawTextW(mDC, tmp.c_str(), -1, &drwRect, 0L);
		os::win32::bitmap::create_bitmap(_buffer, mDC, hBitMap, {(unsigned int)bmpInfo.bmiHeader.biWidth, (unsigned int)bmpInfo.bmiHeader.biHeight});
		for (auto& c : _buffer)
		{
			int	f	= c.r;
			int	e	= c.g;
			c.r	= ((int)mFontColor.r * f + (int)mEdgeColor.r * e) / 255;
			c.g	= ((int)mFontColor.g * f + (int)mEdgeColor.g * e) / 255;
			c.b	= ((int)mFontColor.b * f + (int)mEdgeColor.b * e) / 255;
			c.a	= ((int)mFontColor.a * f + (int)mEdgeColor.a * e) / 255;
		}
		SelectObject(mDC, oldBMP);
		DeleteObject(hBitMap);

		return	{};
	}

#endif
	void release()
	{
		if (mFont)
		{
			if (mPrevious)
			{
				SelectObject(mDC, mPrevious);
				mPrevious	= nullptr;
			}
			DeleteObject(mFont);
			mFont	= nullptr;
		}
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
	HGDIOBJ	hFont	= ::GetStockObject(DEFAULT_GUI_FONT);
	if (!hFont)
	{
		os::win32::throw_system_error();
	}

	LOGFONT	logFont;
	ZeroMemory(&logFont, sizeof(logFont));
	::GetObject(hFont, sizeof(logFont), &logFont);

	if (_default_size <= 0)
	{
		_default_size	= -logFont.lfHeight;
	}
	
	ref<win32_font>	f	= new win32_font(logFont.lfFaceName, _default_size, _default_color);
	return	f;
}


inline ref<font> font::create(const char* _fontname, font_size_type _default_size, pixel_type _default_color)
{
	ref<win32_font>	f	= new win32_font(_fontname, _default_size, _default_color);
	return	f;
}

inline auto font::system_font_size() -> font_size_type
{
	TEXTMETRIC	tm;
	GetTextMetrics(GetDC(NULL), &tm);
	return	static_cast<font_size_type>(tm.tmHeight);
}





AMTRS_NAMESPACE_END
#endif
