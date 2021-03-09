/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__streamif__iostream__hpp
#define	__libamtrs__io__streamif__iostream__hpp
#include <ios>
#include <sstream>
#include <iostream>
AMTRS_IO_NAMESPACE_BEGIN


template<class Elm>
struct	streamif_traits<std::basic_istream<Elm>, Elm>
{
	using	char_type	= Elm;
	using	value_type	= std::basic_istream<Elm>;
	using	size_type	= std::streamsize;
	using	fpos_type	= std::fpos<typename std::char_traits<char_type>::state_type>;


protected:
	streamif_traits(std::basic_istream<Elm>*)
	{}

	streamif_base::iostate read(value_type& _value, size_type& _readsize, char_type* _data, size_type _size)
	{
		size_type	rs	= 0;
		while (_value.good() && _size > rs)
		{
			_value.read(_data, _size - rs);
			rs += _value.gcount();
		}
		_readsize	= rs;
		return	_value.rdstate();
	}

	streamif_base::iostate write(value_type& _value, size_type& _readsize, char_type const* _data, size_type _size)
	{
		return	streamif_base::failbit;
	}

	streamif_base::iostate seek(value_type& _value, fpos_type& _position, streamif_base::seekdir _org)
	{
		_value.seekg(_position, _org);
		//_value.seekp(_position, _org);
		return	_value.rdstate();
	}

	streamif_base::iostate tell(value_type& _value, fpos_type& _position)
	{
		_position	= _value.tellg();
		return	streamif_base::goodbit;
	}

	void clear(value_type& _value, streamif_base::iostate _clear)
	{
		_value.clear(_clear);
	}
};


template<class Source, base_if<std::basic_istream<char>, typename std::remove_pointer<Source>::type> = 0>
auto make_streamif(Source _value)
{
	return	make_basic_streamif<Source, char, std::basic_istream<char>>(std::move(_value));
}








AMTRS_IO_NAMESPACE_END
#endif
