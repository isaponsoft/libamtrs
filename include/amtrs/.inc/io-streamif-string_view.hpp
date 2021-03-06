﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__streamif__string_view__hpp
#define	__libamtrs__io__streamif__string_view__hpp
#include <ios>
#include <sstream>
#include <iostream>
AMTRS_IO_NAMESPACE_BEGIN



template<class Elm>
struct	streamif_traits<std::basic_string_view<Elm>, Elm>
{
	using	char_type	= Elm;
	using	value_type	= std::basic_string_view<Elm>;
	using	size_type	= std::streamsize;
	using	fpos_type	= std::fpos<typename std::char_traits<Elm>::state_type>;

protected:
	streamif_traits(std::basic_string_view<Elm>*)
		: cur(0)
	{}

	streamif_base::iostate read(value_type& _value, size_type& _readsize, char_type* _data, size_type _size)
	{
		streamif_base::iostate	bits	= streamif_base::goodbit;
		size_type				sz		= std::min<size_type>(_size, _value.size() - cur);
		std::copy_n(_value.data() + cur, sz, _data);
		cur			+= sz;
		_readsize	=  sz;
		if (cur == _value.size())
		{
			bits	= streamif_base::eofbit;
		}
		return	bits;
	}

	streamif_base::iostate seek(value_type& _value, fpos_type& _position, streamif_base::seekdir _org)
	{
		switch (_org)
		{
			case streamif_base::beg :
			{
				cur	= _position;
				break;
			}

			case streamif_base::cur :
			{
				cur	+= _position;
				break;
			}

			case streamif_base::end :
			{
				cur	= _value.size() + _position;
				break;
			}

			default :
			{
				return	streamif_base::failbit;
			}
		}
		cur			= std::max<fpos_type>(cur, 0);
		cur			= std::min<fpos_type>(cur, _value.size());
		_position	= cur;
		return	streamif_base::goodbit;
	}

	streamif_base::iostate tell(value_type& _value, fpos_type& _position)
	{
		_position	= cur;
		return	streamif_base::goodbit;
	}

	streamif_base::iostate size(value_type& _value, size_type& _size)
	{
		_size	= _value.size();
		return	streamif_base::goodbit;
	}

	fpos_type	cur;
};

using	stringview_streamif	= basic_streamif<std::string_view, char>;

template<class Source, base_if<std::string_view, Source> = 0>
auto make_streamif(Source _value)
{
	return	make_basic_streamif<Source, char>(std::move(_value));
}


AMTRS_IO_NAMESPACE_END
#endif
