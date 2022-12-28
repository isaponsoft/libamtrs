/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/graphics/image-bmp.hpp>
#include <amtrs/memory/endian.hpp>
AMTRS_GRAPHICS_NAMESPACE_BEGIN


bmp::bmp(void const* _buffer, size_t _size)
{
	parse(_buffer, _size);
}


bool bmp::parse(void const* _buffer, size_t _size)
{
	if (_size < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
	{
		return	false;
	}

	size_t			siz	= _size;
	uint8_t const*	src	= reinterpret_cast<uint8_t const*>(_buffer);

	std::copy_n(src, sizeof(file), reinterpret_cast<uint8_t*>(&file));

	src	+= sizeof(file);
	siz	-= sizeof(file);
	if (file.identify[0] != 'B' || file.identify[1] != 'M')
	{
		AMTRS_WARN_LOG("bmp file has not BM.");
		return	false;
	}
	file.file_size	= endian_util::encode<endian::native>(file.file_size);
	file.offset		= endian_util::encode<endian::native>(file.offset);
	if (_size < file.file_size)
	{
		AMTRS_WARN_LOG("bmp file size or offset error.");
		return	false;
	}

	std::copy_n(src, sizeof(info), reinterpret_cast<uint8_t*>(&info));
	src	+= sizeof(info);
	siz	-= sizeof(info);
	info.info_size			= endian_util::encode<endian::native>(info.info_size);
	info.width				= endian_util::encode<endian::native>(info.width);
	info.height				= endian_util::encode<endian::native>(info.height);
	info.plane				= endian_util::encode<endian::native>(info.plane);
	info.bit_count			= endian_util::encode<endian::native>(info.bit_count);
	info.compress			= endian_util::encode<endian::native>(info.compress);
	info.pixel_size			= endian_util::encode<endian::native>(info.pixel_size);
	info.x_pels_per_meter	= endian_util::encode<endian::native>(info.x_pels_per_meter);
	info.y_pels_per_meter	= endian_util::encode<endian::native>(info.y_pels_per_meter);
	info.color_used			= endian_util::encode<endian::native>(info.color_used);
	info.color_impotant		= endian_util::encode<endian::native>(info.color_impotant);
	if (siz < info.info_size)
	{
		AMTRS_WARN_LOG("bmp file info_size error.");
		return	false;
	}

	switch (info.bit_count)
	{
		case 24 :
		{
			auto	pix	= shared_buffer<rgb<uint8_t>>(info.width * info.height);
			auto	dst	= pix.data() + info.width * info.height;
			for (unsigned int h = 0; h < info.height; ++h)
			{
				dst -= info.width;
				auto	d	= dst;
				for (unsigned int x = 0; x < info.width; ++x)
				{
					d->b	= *src++;
					d->g	= *src++;
					d->r	= *src++;
					++d;
				}
			}
			pixels	= std::move(pix);
			return	true;
		}

		case 32 :
		{
			auto	pix	= shared_buffer<rgba<uint8_t>>(info.width * info.height);
			auto	dst	= pix.data() + info.width * info.height;
			for (unsigned int h = 0; h < info.height; ++h)
			{
				dst -= info.width;
				auto	d	= dst;
				for (unsigned int x = 0; x < info.width; ++x)
				{
					d->b	= *src++;
					d->g	= *src++;
					d->r	= *src++;
					d->a	= *src++;
					++d;
				}
			}
			pixels	= std::move(pix);
			return	true;
		}
	}

	AMTRS_WARN_LOG("Unkown bitmap file bit size {}", info.bit_count);
	return	false;
}


bmp& bmp::to_rgba(uint8_t _alpha)
{
	switch (info.bit_count)
	{
		case 24 :
		{
			auto	pix	= shared_buffer<rgba<uint8_t>>(info.width * info.height);
			auto	dst	= pix.data() + info.width * info.height;
			auto	src	= reinterpret_cast<rgb<uint8_t> const*>(pixels.data());
			for (unsigned int h = 0; h < info.height; ++h)
			{
				dst -= info.width;
				auto	d	= dst;
				for (unsigned int h = 0; h < info.height; ++h)
				{
					d->r	= src->r;
					d->g	= src->g;
					d->b	= src->b;
					d->a	= _alpha;
					++d;
					++src;
				}
			}
			pixels	= std::move(pix);
			break;
		}
	}
	return	*this;
}





AMTRS_GRAPHICS_NAMESPACE_END
