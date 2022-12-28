/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/io.hpp>
#include <amtrs/graphics.hpp>
#include <amtrs/filesystem.hpp>
#include <iostream>
AMTRS_NAMESPACE_BEGIN


imagebuff<uint8_t> create_imagebuff(imageview<void const> _view, color_format _color)
{
	auto				px(pixel_size(_color));
	imagebuff<uint8_t>	r(_view.size());
	bool				copy_x	= _view.stride().x == px;
	bool				copy_y	= _view.stride().y == px * _view.size().width;

	uint8_t*			dest	= r.data();
	if (copy_x)
	{
		if (copy_y)
		{
			// すべて連続しているデータなのでそのままコピー
			std::memcpy(dest, (uint8_t const*)_view.data(), px * _view.size().width * _view.size().height);
		}
		else
		{
			// Y単位でコピー
			void const*	src		= _view.data();
			size_t		xsize	= px * _view.size().width;
			for (int yc = _view.size().height; yc > 0; --yc)
			{
				std::memcpy(dest, src, xsize);
				src		= reinterpret_cast<decltype(src )>( reinterpret_cast<std::intptr_t>(src ) + _view.stride().y );
				dest	= reinterpret_cast<decltype(dest)>( reinterpret_cast<std::intptr_t>(dest) + xsize            );
			}
		}
	}
	else
	{
		if (px == 1)
		{
			imageview<uint8_t>			d(r);
			imageview<uint8_t const>	s(_view);
			std::copy(s.begin(), s.end(), d.begin());
		}
		else if (px == 2)
		{
			imageview<uint16_t>			d(r);
			imageview<uint16_t const>	s(_view);
			std::copy(s.begin(), s.end(), d.begin());
		}
		else if (px == 4)
		{
			imageview<uint32_t>			d(r);
			imageview<uint32_t const>	s(_view);
			std::copy(s.begin(), s.end(), d.begin());
		}
		else if (px == 8)
		{
			imageview<uint64_t>			d(r);
			imageview<uint64_t const>	s(_view);
			std::copy(s.begin(), s.end(), d.begin());
		}
		else
		{
			AMTRS_DEBUG_LOG("%s", "Unsupported pixel size error");
		}
	}
	return	r;
}

auto loadimage_traits<rgba<uint8_t>>::load(std::string_view _filename, filesystem::fileloader* _loader) -> image_type
{
	ref<filesystem::fileloader>	defloader;
	if (!_loader)
	{
		defloader	= filesystem::fileloader::get_instance();
		_loader		= defloader;
	}

	std::error_code	ec;
	auto			in		= _loader->open(_filename, ec);
	if (ec)
	{
		return	{};
	}
	auto	bmp		= basic_bitmap<rgba<uint8_t>, shared_buffer<rgba<uint8_t>>>::load(in);
	if (bmp.empty())
	{
		return	{};
	}
	return	image_type(bmp.size(), bmp.pixels().data());
}


AMTRS_NAMESPACE_END
