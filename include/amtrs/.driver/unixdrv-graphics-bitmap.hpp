/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__bitmaps__bitmap_ios__hpp
#define	__libamtrs__bitmaps__bitmap_ios__hpp
#include <iostream>
#include "../io.hpp"
#ifdef	AMTRS_JPEG_ENABLE
#include AMTRS_DRIVER_CUSTOM_INCLUDE(image/libjpeg.hpp)
#endif
#ifdef	AMTRS_PNG_ENABLE
#include AMTRS_DRIVER_CUSTOM_INCLUDE(image/libpng.hpp)
#endif
AMTRS_NAMESPACE_BEGIN


template<class PixelT, class BufferT>
template<class In>
auto basic_bitmap<PixelT, BufferT>::load(In&& _in) -> basic_bitmap
{
	// ファイルサイズ取得＆バッファに読み込み
	auto		in			= io::make_rstream(std::forward<In>(_in));
	auto		inSize		= static_cast<std::size_t>(in.seekg(0, std::ios_base::end).tellg());
	in.seekg(0, std::ios_base::beg);

	std::vector<char>	buff(inSize);
	in.read(buff.data(), inSize);

	bool							success	= false;
	basic_bitmap<PixelT, BufferT>	retval;

#ifdef	AMTRS_JPEG_ENABLE
	retval	= libjpeg::load_jpeg<PixelT, BufferT>(&success, buff.data(), buff.size());
	if (success)
	{
		return	retval;
	}
#endif

#ifdef	AMTRS_PNG_ENABLE
	if (libpng::has_png_header(buff.data(), buff.size()))
	{
		return	libpng::png_load<PixelT, BufferT>(buff.data(), buff.size());
	}
#endif

	return	basic_bitmap<PixelT, BufferT>();
}


AMTRS_NAMESPACE_END
#endif
