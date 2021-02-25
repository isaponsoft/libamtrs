/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <cstring>
#if		__has_include(<png.h>)
#include <png.h>
#elif	__has_include(<png/png.h>)
#include <png/png.h>
#endif
AMTRS_NAMESPACE_BEGIN namespace libpng {

inline bool has_png_header(char const* _data, size_t _size)
{
	if (_size < 8)
	{
		return	false;
	}

	static const unsigned char	header[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	if (std::memcmp(header, _data, 8))
	{
		return	false;
	}
	return	true;
}


template<class PixelT, class BufferT>
basic_bitmap<PixelT, BufferT> png_load(char const* _data, size_t _size)
{
	using	size_type	= typename basic_bitmap<PixelT, BufferT>::size_type;

	struct	memstream
	{
		unsigned char const*	data;
		size_t					data_size;
		unsigned long			cursor;

		static void read(png_structp _png, png_bytep _buffer, png_size_t _read_size)
		{
			memstream*	thiz	= (memstream*)png_get_io_ptr(_png);
			if (thiz->data_size >= (thiz->cursor + _read_size))
			{
				std::memcpy(_buffer, thiz->data + thiz->cursor, _read_size);
				thiz->cursor += _read_size;
			}
		}
	};

	basic_bitmap<PixelT, BufferT>	retval;
	png_struct*						pngStruct	= nullptr;
	png_info*						pngInfo		= nullptr;
	memstream						stream;


	pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!pngStruct)
	{
		return	{};
	}

	pngInfo = png_create_info_struct(pngStruct);
	if (!pngInfo)
	{
		png_destroy_read_struct(&pngStruct, nullptr, nullptr);
		return	{};
	}

	stream.data			= (unsigned char const*)_data;
	stream.data_size	= _size;
	stream.cursor		= 0;
	png_set_read_fn(pngStruct, &stream, (png_rw_ptr)&memstream::read);



	unsigned int	width, height;
	int				depth, colorType;
	int				interlace, compType, filterType;

	png_read_info(pngStruct, pngInfo);
	png_get_IHDR(pngStruct, pngInfo, &width, &height, &depth, &colorType, &interlace, &compType, &filterType);

	png_set_gray_to_rgb(pngStruct);			// グレースケールの色はRGBに変換する
	png_set_palette_to_rgb(pngStruct);		// パレットカラーもRGBに変換する
	//png_set_bgr(pngStruct);
	//png_set_packing(pngStruct);

	png_set_add_alpha(pngStruct, 0xff, PNG_FILLER_AFTER);
//	png_set_invert_alpha(m_pngStruct);			// png_set_add_alpha(m_pngStruct); add_alphaは単なる 0 で埋められるのでお勧めしない
	png_set_strip_16(pngStruct);				// 色の深さを 16bit諧調では無く 8bit諧調にする

//	png_set_filler(m_pngStruct, 8, PNG_FILLER_AFTER);

	png_read_update_info(pngStruct, pngInfo);
	png_get_IHDR(pngStruct, pngInfo, &width, &height, &depth, &colorType, &interlace, &compType, &filterType);


	retval	= basic_bitmap<PixelT, BufferT>(size_type(width, height));

//	png_start_read_image(pngStruct);
	png_uint_32	nLine			= 0;
	png_bytep	pDest			= (png_bytep)retval.pixels().data();
	while (nLine < height)
	{
		png_read_row(pngStruct, pDest, nullptr);
		pDest += width * depth * 4 / 8;
		++nLine;
	}
//	png_read_end(pngStruct, nullptr);

	png_destroy_read_struct(&pngStruct, &pngInfo, nullptr);

	return	retval;
}



} AMTRS_NAMESPACE_END
