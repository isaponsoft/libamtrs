/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__streamif__iostream__hpp
#define	__libamtrs__io__streamif__iostream__hpp
#include <ios>
#include <sstream>
#include <iostream>
AMTRS_IO_NAMESPACE_BEGIN


template<>
struct	streamif_traits<std::istream>
{
	using	stream_type	= std::istream;
	using	size_type	= streamif_base::size_type;
	using	pos_type	= streamif_base::pos_type;


protected:
	streamif_base::iostate read(stream_type& _value, size_type& _readsize, void* _data, size_type _size)
	{
		size_type	rs	= 0;
		while (_value.good() && _size > rs)
		{
			_value.read((char*)_data, _size - rs);
			rs += _value.gcount();
		}
		_readsize	= rs;
		return	_value.rdstate();
	}

	streamif_base::iostate write(stream_type& _value, size_type& _readsize, void const* _data, size_type _size)
	{
		return	streamif_base::failbit;
	}

	streamif_base::iostate seek(stream_type& _value, pos_type& _position, streamif_base::seekdir _org)
	{
		_value.seekg(_position, _org);
		//_value.seekp(_position, _org);
		return	_value.rdstate();
	}

	streamif_base::iostate tell(stream_type& _value, pos_type& _position)
	{
		_position	= _value.tellg();
		return	streamif_base::goodbit;
	}

	void clear(stream_type& _value, streamif_base::iostate _clear)
	{
		_value.clear((std::ios_base::iostate)_clear);
	}
};


template<class Source, base_if<std::istream, typename std::remove_pointer<Source>::type> = 0>
auto make_streamif(Source&& _value)
{
	return	streamif<Source, streamif_traits<std::istream>>(std::move(_value));
}








AMTRS_IO_NAMESPACE_END
#endif
