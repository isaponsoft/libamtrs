/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__gif__header__hpp
#define	__libamtrs__graphics__gif__header__hpp
#include "../../io/read.hpp"
#include "../../io/io_traits-view.hpp"
#include "../../io/stream_in-view.hpp"
#include "def.hpp"
AMTRS_GRAPHICS_GIF_NAMESPACE_BEGIN



static constexpr uint8_t	trailer	= 0x3b;

enum class gifversion
{
	gif87a,
	gif89a,
};

struct	fileheader
{
	static constexpr size_t	file_size	= 13;

	char		signature[3];			// "GIF"
	char		version[3];				// "87a" "89a"
	uint16_t	logical_width;
	uint16_t	logical_height;
	uint8_t		packedFields;
	uint8_t		backgroundColorIndex;
	uint8_t		pixelAspectRatio;


	bool gif87a() const noexcept { return !std::memcmp(version, "87a", 3); }
	bool gif89a() const noexcept { return !std::memcmp(version, "89a", 3); }

	constexpr bool has_global_color_table() const noexcept { return (packedFields>>7) & 0x01; }
	constexpr size_t color_resolution() const noexcept { return ((packedFields>>4) & 0x07) + 1; }
	constexpr bool palette_sorted() const noexcept { return (packedFields>>3) & 0x01; }
	constexpr size_t size_of_global_color_table() const noexcept { return has_global_color_table() ? (1 << ((packedFields & 0x07) +1)) : 0; }

	constexpr float pixel_aspect_ratio() const noexcept { return (float)(pixelAspectRatio+15) / 64.0f; }
};


struct	color
{
	uint8_t	r;
	uint8_t	g;
	uint8_t	b;
};


struct	image_block
{
	static constexpr uint8_t	separator_code	= 0x2c;

	uint16_t		imageLeftPosition;
	uint16_t		imageTopPosition;
	uint16_t		imageWidth;
	uint16_t		imageHeight;
	uint8_t			packedFields;

	constexpr bool has_local_color_table() const noexcept { return (packedFields>>7)&0x01; }
	constexpr bool interlace() const noexcept { return (packedFields>>6)&0x01; }
	constexpr bool sorted() const noexcept { return (packedFields>>5)&0x01; }
	constexpr size_t size_of_local_color_table() const noexcept { return has_local_color_table() ? 1 << ((packedFields&0x07)+1) : 0; }

};


struct	extenssion
{
	uint8_t			introducer;
	uint8_t			label;
	uint8_t			blockSize;
};


struct	controll_extenssion
{
	static constexpr uint8_t	separator_code	= 0x21;
	static constexpr uint8_t	label_code		= 0xf9;

	uint8_t						introducer;			//!< = separator
	uint8_t						label;				//!< = separator
	uint8_t						blockSize;			//!< 0x04

	uint8_t						flags;
	uint16_t					delayTime;
	uint8_t						transparentColorIndex;


	constexpr int disposal_method() const noexcept { return (flags>>2) & 0x07; }
	constexpr bool user_input_flag() const noexcept { return (flags>>1) & 0x01; }
	constexpr bool transparent_color_flag() const noexcept { return (flags>>0) & 0x01; }
	constexpr float delay_sec() const noexcept { return (float)delayTime / 100.0f; }
};


struct	comment_extenssion
{
	static constexpr uint8_t	separator_code	= 0x21;
	static constexpr uint8_t	label_code		= 0xfe;
};


struct	plain_extenssion
{
	static constexpr uint8_t	separator_code	= 0x21;
	static constexpr uint8_t	label_code		= 0x01;
};


struct	application_extenssion
{
	static constexpr uint8_t	separator_code	= 0x21;
	static constexpr uint8_t	label_code		= 0xff;

	uint8_t						introducer;			//!< = separator
	uint8_t						label;				//!< = separator
	uint8_t						blockSize;			//!< 0x0b
	char						identifier[8];
	char						authentication[3];
};



AMTRS_GRAPHICS_GIF_NAMESPACE_END

AMTRS_IO_NAMESPACE_BEGIN

template<class In>
bool read(gif::fileheader& _out, In&& _in)
{
	char	buff[gif::fileheader::file_size];
	if (io::read(buff, std::forward<In>(_in)) != sizeof(buff))
	{
		return	false;
	}

	auto	i	= io::make_stream_in(make_view(buff));
	io::read(_out.signature, i);
	if (std::memcmp(_out.signature, "GIF", 3))
	{
		return	false;
	}
	if (io::read(_out.version, i) != sizeof(_out.version))
	{
		return	false;
	}
	io::read<endian::little>(_out.logical_width,		i);
	io::read<endian::little>(_out.logical_height,		i);
	io::read<endian::little>(_out.packedFields,			i);
	io::read<endian::little>(_out.backgroundColorIndex,	i);
	io::read<endian::little>(_out.pixelAspectRatio,		i);
	return	true;
}


template<class In>
bool read(gif::color& _out, In&& _in)
{
	io::read<endian::little>(_out.r, _in);
	io::read<endian::little>(_out.g, _in);
	io::read<endian::little>(_out.b, _in);
	return	true;
}


template<class In>
bool read(gif::image_block& _out, In&& _in)
{
	io::read<endian::little>(_out.imageLeftPosition,	_in);
	io::read<endian::little>(_out.imageTopPosition,		_in);
	io::read<endian::little>(_out.imageWidth,			_in);
	io::read<endian::little>(_out.imageHeight,			_in);
	io::read<endian::little>(_out.packedFields,			_in);
	return	true;
};


template<class In, class DataCallback>
bool read(gif::extenssion& _out, In&& _in, bool _skipIntro)
{
	if (!_skipIntro)
	{
		io::read<endian::little>(_out.introducer,	_in);
	}
	io::read<endian::little>(_out.label,		_in);
	io::read<endian::little>(_out.blockSize,	_in);
	return	true;
}


template<class In, class DataCallback>
bool read(gif::application_extenssion& _out, In&& _in, bool _skipIntro, DataCallback&& _callback = [](size_t) -> char*{ return nullptr; })
{
	if (!_skipIntro)
	{
		io::read<endian::little>(_out.introducer,	_in);
		io::read<endian::little>(_out.label,		_in);
	}
	io::read<endian::little>(_out.blockSize,		_in);
	io::read(_out.identifier,		_in);
	io::read(_out.authentication,	_in);

	while (_in.good())
	{
		uint8_t	bs;
		io::read<endian::little>(bs, _in);
		if (bs == 0)
		{
			break;
		}
		char*	buff	= _callback(static_cast<size_t>(bs));
		char	dummy[256];
		if (!buff)
		{
			buff	= dummy;
		}
		io::read(buff, _in, static_cast<size_t>(bs));
	}

	return	true;
}


template<class In>
bool read(gif::controll_extenssion& _out, In&& _in, bool _skipIntro)
{
	if (!_skipIntro)
	{
		io::read<endian::little>(_out.introducer,	_in);
		io::read<endian::little>(_out.label,		_in);
	}
	io::read<endian::little>(_out.blockSize,				_in);
	io::read<endian::little>(_out.flags,					_in);
	io::read<endian::little>(_out.delayTime,				_in);
	io::read<endian::little>(_out.transparentColorIndex,	_in);
	uint8_t	zero;
	io::read<endian::little>(zero, _in);
	return	true;
}


AMTRS_IO_NAMESPACE_END
#endif
