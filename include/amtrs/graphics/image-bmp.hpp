/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__image_bmp__hpp
#define	__libamtrs__graphics__image_bmp__hpp
#include "../memory/shared_buffer.hpp"
#include "geometory/size.hpp"
#include "color.hpp"
AMTRS_GRAPHICS_NAMESPACE_BEGIN

struct	bmp
{
	#pragma pack(1)
	struct	BITMAPFILEHEADER
	{
		char		identify[2];		// 'B' 'M'
		uint32_t	file_size;
		uint16_t	reserve1;
		uint16_t	reserve2;
		uint32_t	offset;
	};// __attribute__((__packed__));


	struct	BITMAPINFOHEADER
	{
		uint32_t	info_size;
		uint32_t	width;
		uint32_t	height;
		uint16_t	plane;
		uint16_t	bit_count;
		uint32_t	compress;
		uint32_t	pixel_size;
		uint32_t	x_pels_per_meter;
		uint32_t	y_pels_per_meter;
		uint32_t	color_used;
		uint32_t	color_impotant;
	};// __attribute__((__packed__));

	#pragma pack()


	BITMAPFILEHEADER				file;
	BITMAPINFOHEADER				info;
	shared_buffer<rgba<uint8_t>>	palette;
	shared_buffer<void>				pixels;

	bmp() = default;
	bmp(bmp const&) = default;
	bmp(bmp&&) = default;
	bmp& operator = (bmp const&) = default;
	bmp& operator = (bmp&&) = default;

	bmp(void const* _buffer, size_t _size);

	// bmp file parse from buffer.
	bool parse(void const* _buffer, size_t _size);

	bool empty() const noexcept { return pixels.empty(); }
	auto size() const noexcept -> size2<unsigned int> { return { info.width, info.height }; }

	bmp& to_rgba(uint8_t _alpha = 255);
};

AMTRS_GRAPHICS_NAMESPACE_END
#endif

