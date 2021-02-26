/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__io_traits_stl__hpp
#define	__libamtrs__io__io_traits_stl__hpp
#include <iosfwd>
AMTRS_IO_NAMESPACE_BEGIN


template<class CharT, class Traits>
struct	io_traits<std::basic_iostream<CharT, Traits>>
{
	using	stream_type	= std::basic_istream<CharT, Traits>;
	using	char_type	= typename stream_type::char_type;


	size_t read(void* _buff, stream_type& _io, size_t _size)
	{
		_io.read(reinterpret_cast<char_type*>(_buff), _size);
		auto	r	= _io.gcount();
		return	r;
	}


	size_t write(stream_type& _io, const void* _buff, size_t _size)
	{
		_io.write(reinterpret_cast<const char_type*>(_buff), _size);
		auto	r	= _io.gcount();
		return	r;
	}



	bool seekg(stream_type& _io, std::ios::off_type _off, std::ios::seekdir _dir)
	{
		_io.seekg(_off, _dir);
		return	true;
	}


	bool seekp(stream_type& _io, std::ios::off_type _off, std::ios::seekdir _dir)
	{
		_io.seekp(_off, _dir);
		return	true;
	}


	uintmax_t size(stream_type& _io)
	{
		_io.clear();
		seekg(_io, 0, std::ios::end);
		auto	endPos	= _io.tellg();
		_io.clear();
		seekg(_io, 0, std::ios::beg);
		auto	begPos	= _io.tellg();
		_io.clear();
		return	static_cast<uintmax_t>(endPos - begPos);
	}
};


template<class CharT, class Traits>
struct	io_traits<std::basic_fstream<CharT, Traits>>
		: io_traits<std::basic_iostream<CharT, Traits>>
{};



AMTRS_IO_NAMESPACE_END
#endif
