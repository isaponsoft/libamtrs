/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if	0
#ifndef	__libamtrs__bitmaps__bitmap_win32__hpp
#define	__libamtrs__bitmaps__bitmap_win32__hpp
#include <atomic>
#include <iostream>
#include "../io.hpp"
#include "win32-api-windows.hpp"
#include "win32-api-com_istream.hpp"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
AMTRS_NAMESPACE_BEGIN

inline const char* get_gdiplus_pixel_format_str(Gdiplus::PixelFormat _format);


template<class PixelT, class BufferT>
template<class In>
auto basic_bitmap<PixelT, BufferT>::load(In _in) -> basic_bitmap
{
	basic_bitmap	retval;

	Gdiplus::GdiplusStartupInput	gpSI;
	ULONG_PTR			lpToken;
	Gdiplus::GdiplusStartup(&lpToken, &gpSI, nullptr);
	{
		auto											rs	= io::make_streamif(_in);
		os::win32::com_istream_wrapper<decltype(rs)>	ist(rs);
		Gdiplus::Bitmap									image(&ist);
		if (auto err = image.GetLastStatus(); err != Gdiplus::Ok)
		{
			AMTRS_DEBUG_LOG("Bitmap load error. %d", err);
			throw std::exception("Bitmap load error");
		}

		UINT				width	= image.GetWidth();
		UINT				height	= image.GetHeight();

		retval	= basic_bitmap(size_type(width, height));


		Gdiplus::BitmapData	bitmapData;
		Gdiplus::Rect		rect(0, 0, width, height);
		if (PixelFormat32bppARGB == image.GetPixelFormat())
		{
			if (auto status = image.LockBits(
				&rect,
				Gdiplus::ImageLockModeRead,
				PixelFormat32bppARGB,
				&bitmapData
			); status != Gdiplus::Ok)
			{
				AMTRS_DEBUG_LOG("Bitmap 32bpp lock error.");
				throw std::exception("Bitmap 32bpp lock error");
			}
			UINT*	src	= (UINT*)bitmapData.Scan0;
			for (auto& p : retval.pixels())
			{
				p.r	= ((*src) & 0x00ff0000) >> 16;
				p.g	= ((*src) & 0x0000ff00) >>  8;
				p.b	= ((*src) & 0x000000ff) >>  0;
				p.a	= ((*src) & 0xff000000) >> 24;
				++src;
			}
		}
		else if (PixelFormat24bppRGB == image.GetPixelFormat())
		{
			if (auto status = image.LockBits(
				&rect,
				Gdiplus::ImageLockModeRead,
				PixelFormat24bppRGB,
				&bitmapData
			); status != Gdiplus::Ok)
			{
				// 画像がベースライン形式以外で保存されている可能性がある
				throw std::exception("Bitmap 24bpp lock error");
			}

			auto*		p	= retval.pixels().data();
			for (UINT y = 0; y < height; ++y)
			{
				auto	line	= (uint8_t*)bitmapData.Scan0 + y * bitmapData.Stride;
				for (UINT x = 0; x < width; ++x)
				{
					p->b	= (*line++);
					p->g	= (*line++);
					p->r	= (*line++);
					p->a	= 0xff;
					++p;
				}
			}
		}
		else if (PixelFormat8bppIndexed == image.GetPixelFormat())
		{
			auto	pltSize	= image.GetPaletteSize();
			auto	buff	= new char [pltSize];
			auto	plt		= reinterpret_cast<Gdiplus::ColorPalette*>(buff);

			if (auto status = image.GetPalette(plt, pltSize); status != Gdiplus::Ok)
			{
				AMTRS_DEBUG_LOG("Bitmap palette error");
				delete[] buff;
				throw std::exception("Bitmap 8bpp Indexed palette error");
			}

			if (auto status = image.LockBits(
				&rect,
				Gdiplus::ImageLockModeRead,
				PixelFormat8bppIndexed,
				&bitmapData
			); status != Gdiplus::Ok)
			{
				// 画像がベースライン形式以外で保存されている可能性がある
				delete[] buff;
				throw std::exception("Bitmap 8bpp Indexed lock error");
			}

			uint8_t*	src	= (uint8_t*)bitmapData.Scan0;
			for (auto it = retval.pixels().begin(); it != retval.pixels().end(); ++src)
			{
				auto	c1	= plt->Entries[*src];
				it->r	= static_cast<uint8_t>(c1>>RED_SHIFT);
				it->g	= static_cast<uint8_t>(c1>>GREEN_SHIFT);
				it->b	= static_cast<uint8_t>(c1>>BLUE_SHIFT);
				it->a	= static_cast<uint8_t>(c1>>ALPHA_SHIFT);
				++it;
			}
			delete[] buff;
		}
		else if (PixelFormat4bppIndexed == image.GetPixelFormat())
		{
			auto	pltSize	= image.GetPaletteSize();
			auto	buff	= new char [pltSize];
			auto	plt		= reinterpret_cast<Gdiplus::ColorPalette*>(buff);

			if (auto status = image.GetPalette(plt, pltSize); status != Gdiplus::Ok)
			{
				AMTRS_DEBUG_LOG("Bitmap palette error");
				delete[] buff;
				throw std::exception("Bitmap 8bpp Indexed palette error");
			}

			if (auto status = image.LockBits(
				&rect,
				Gdiplus::ImageLockModeRead,
				PixelFormat4bppIndexed,
				&bitmapData
			); status != Gdiplus::Ok)
			{
				// 画像がベースライン形式以外で保存されている可能性がある
				delete[] buff;
				throw std::exception("Bitmap 8bpp Indexed lock error");
			}

			uint8_t*	src	= (uint8_t*)bitmapData.Scan0;
			for (auto it = retval.pixels().begin(); it != retval.pixels().end(); ++src)
			{
				auto	c1	= plt->Entries[(*src >> 4) & 0x0f];
				auto	c2	= plt->Entries[(*src     ) & 0x0f];
				it->r	= static_cast<uint8_t>(c1>>RED_SHIFT);
				it->g	= static_cast<uint8_t>(c1>>GREEN_SHIFT);
				it->b	= static_cast<uint8_t>(c1>>BLUE_SHIFT);
				it->a	= static_cast<uint8_t>(c1>>ALPHA_SHIFT);
				++it;
				it->r	= static_cast<uint8_t>(c2>>RED_SHIFT);
				it->g	= static_cast<uint8_t>(c2>>GREEN_SHIFT);
				it->b	= static_cast<uint8_t>(c2>>BLUE_SHIFT);
				it->a	= static_cast<uint8_t>(c2>>ALPHA_SHIFT);
				++it;
			}
			delete[] buff;
		}
		else
		{
			AMTRS_DEBUG_LOG("Bitmap unsupported format.");
			throw std::exception("Bitmap unsupported format");
		}
		image.UnlockBits(&bitmapData);
    }
	Gdiplus::GdiplusShutdown(lpToken);

	return	retval;
}


const char* get_gdiplus_pixel_format_str(Gdiplus::PixelFormat _format)
{
	#define	__(_e)	case _e : return #_e;
	switch (_format)
	{
		__(PixelFormat1bppIndexed)
		__(PixelFormat4bppIndexed)
		__(PixelFormat8bppIndexed)
		__(PixelFormat16bppARGB1555)
		__(PixelFormat16bppGrayScale)
		__(PixelFormat16bppRGB555)
		__(PixelFormat16bppRGB565)
		__(PixelFormat24bppRGB)
		__(PixelFormat32bppARGB)
		__(PixelFormat32bppPARGB)
		__(PixelFormat32bppRGB)
		__(PixelFormat48bppRGB)
		__(PixelFormat64bppARGB)
		__(PixelFormat64bppPARGB)
	}
	#undef	__
	return	"PixelFormatUnkown";
}

AMTRS_NAMESPACE_END
#endif
#endif
