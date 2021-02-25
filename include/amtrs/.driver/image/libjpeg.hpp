/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <cstring>
#include <jpeglib.h>
AMTRS_NAMESPACE_BEGIN namespace libjpeg {

template<class PixelT, class BufferT>
basic_bitmap<PixelT, BufferT> load_jpeg(bool* _success, char const* _data, size_t _size)
{
	using	size_type	= typename basic_bitmap<PixelT, BufferT>::size_type;

	*_success	= false;

	struct jpeg_decompress_struct	jds;
	struct jpeg_error_mgr			jerr;
	jds.err	= jpeg_std_error(&jerr);
	jpeg_create_decompress(&jds);

	jpeg_mem_src(&jds, reinterpret_cast<unsigned char const*>(_data), static_cast<unsigned long>(_size));

	if (jpeg_read_header(&jds, true) != 1)
	{
		return	{};
	}

	jpeg_start_decompress(&jds);
	if (jds.out_color_space != JCS_RGB)
	{
	}


	basic_bitmap<PixelT, BufferT>	retval(size_type(jds.output_width, jds.output_height));
	std::vector<unsigned char>		buff(jds.output_width * jds.output_components);
	auto*							dest		= retval.pixels().data();
	while (jds.output_scanline < jds.output_height)
	{
		unsigned char*	bufferlines[1] = { buff.data() };
		jpeg_read_scanlines(&jds, bufferlines, 1);

		auto*			src				= buff.data();
		for (int x = 0; x < jds.output_width; ++x)
		{
			dest[x].r	= src[0];
			dest[x].g	= src[1];
			dest[x].b	= src[2];
			dest[x].a	= 255;
			src += jds.output_components;
		}
		dest += jds.output_width;
	}

	jpeg_finish_decompress(&jds);
	jpeg_destroy_decompress(&jds);

	*_success	= true;
	return	retval;
}



} AMTRS_NAMESPACE_END
