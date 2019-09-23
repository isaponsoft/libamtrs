#ifndef	__libamtrs__images__bmp__hpp
#define	__libamtrs__images__bmp__hpp
#include "../iostream/@"
#include "../utility/array.hpp"
#include "def.hpp"
#include "bitmap.hpp"
AMTRS_NAMESPACE_BEGIN

template<class...>
struct	bmp;


template<class PixelT, class ArrayT>
struct	bmp<basic_bitmap<PixelT, ArrayT>>
{
	using	bitmap_type	= basic_bitmap<PixelT, ArrayT>;
	bmp(bitmap_type& _image) : mImage(_image) {}
	bitmap_type&	mImage;
};

template<class PixelT, class ArrayT>
struct	bmp<const basic_bitmap<PixelT, ArrayT>>
{
	using	bitmap_type	= basic_bitmap<PixelT, ArrayT>;
	bmp(const bitmap_type& _image) : mImage(_image) {}
	const bitmap_type&	mImage;
};




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



template<class ImageT>
std::ostream& operator << (std::ostream& _out, bmp<ImageT> _bmp)
{
	using	value_type	= rgba<uint8_t>;
	constexpr uint32_t	BMPFILEHEADER_SIZE	= 14;
	constexpr uint32_t	BMPINFOHEADER_SIZE	= 40;

	const auto&	img	= _bmp.mImage;

	std::size_t	imageBitSize	= img.size().width * img.size().height * img.bitrate();

	// Bitmap file header(16bytes)
	_out << "BM";
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>((unsigned int)(imageBitSize / 8) + BMPFILEHEADER_SIZE + BMPINFOHEADER_SIZE)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint16_t>(0)));		// reserve
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint16_t>(0)));		// reserve
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(BMPFILEHEADER_SIZE + BMPINFOHEADER_SIZE)));		// offset

	// Bitmap info header(40bytes)
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(BMPINFOHEADER_SIZE)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(img.size().width)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(img.size().height)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint16_t>(1)));					// plane
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint16_t>(img.bitrate())));		// bitcount
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(0)));					// compress
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>((unsigned int)imageBitSize / 8)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(0)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(0)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(0)));
	_out << ios::make_bin(make_array(endian_util::encode<endian::little, uint32_t>(0)));


	// Pixel data.
	std::vector<value_type>	buff(img.size().width);
	for (unsigned int y = 0; y < img.size().height; ++y)
	{
		auto	cursor	= img.subimg({0, img.size().height-1-y, img.size().width, 1}).begin();
		auto	dest	= buff.begin();
		for (unsigned int x = 0; x < img.size().width; ++x)
		{
			*dest = *cursor++;
			std::swap(dest->r, dest->b);
			++dest;
		}
		_out.write(reinterpret_cast<const char*>(buff.data()), sizeof(value_type) * buff.size());
	}

	return	_out;
}




template<class ImageT>
std::istream& operator >> (std::istream& _in, bmp<ImageT> _bmp)
{
	auto&	img	= _bmp.mImage;

	BITMAPFILEHEADER	fileHeader;
	BITMAPINFOHEADER	infoHeader;

	_in >> amtrs::ios::make_bin(fileHeader);
	_in >> amtrs::ios::make_bin(infoHeader);
	if (amtrs::make_array(fileHeader.identify) != amtrs::make_array<char>({'B', 'M'}))
	{
		throw	std::runtime_error("Unkown bmp file format");
	}

	img = ImageT({infoHeader.width, infoHeader.height});
	switch (infoHeader.bit_count)
	{
		case 24:
		{
			rgb<uint8_t>	t;
			for (unsigned int y = 0; y < infoHeader.height; ++y)
			{
				auto	cursor	= img.subimg({0, infoHeader.height-1-y, infoHeader.width, 1}).begin();
				for (unsigned int x = 0; x < infoHeader.width; ++x)
				{
					_in >> amtrs::ios::make_bin(t);
					*cursor++ = {t.b, t.g, t.r, 255};	// swap R <=> B.
				}
			}
			break;
		}

		case 32:
		{
			rgba<uint8_t>	t;
			for (unsigned int y = 0; y < infoHeader.height; ++y)
			{
				auto	cursor	= img.subimg({0, infoHeader.height-1-y, infoHeader.width, 1}).begin();
				for (unsigned int x = 0; x < infoHeader.width; ++x)
				{
					_in >> amtrs::ios::make_bin(t);
					std::swap(t.r, t.b);
					*cursor++ = t;	// swap R <=> B.
				}
			}
			break;
		}

		default:
			throw	std::runtime_error("Unsupported bmp file bit count");
	}

	return	_in;
}

AMTRS_NAMESPACE_END
#endif
