﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__streamif__stdlib_hpp
#define	__libamtrs__io__streamif__stdlib_hpp
#include <sys/types.h>
#include <sys/stat.h>
#include <ios>
#include <sstream>
#include <iostream>
AMTRS_IO_NAMESPACE_BEGIN


template<>
struct	streamif_traits<FILE*>
{
	using	stream_type	= FILE*;
	using	size_type	= std::streamsize;
	using	fpos_type	= std::fpos<typename std::char_traits<char>::state_type>;


protected:
	void close(stream_type& _value)
	{
		if (_value)
		{
			::fclose(_value);
			_value	= nullptr;
		}
	}

	streamif_base::iostate read(stream_type& _value, size_type& _readsize, void* _data, size_type _size)
	{
		streamif_base::iostate	bits	= streamif_base::goodbit;
		if (auto s = ::fread(_data, 1, _size, _value); s)
		{
			_readsize	= s;
			return	bits;
		}
		if (::feof(_value))
		{
			bits	= streamif_base::eofbit;
		}
		if (::ferror(_value))
		{
			bits	= streamif_base::failbit;
		}
		::clearerr(_value);
		return	bits;
	}

	streamif_base::iostate seek(stream_type& _value, fpos_type& _position, streamif_base::seekdir _org)
	{
		streamif_base::iostate	rs	= streamif_base::goodbit;
		int	org	= _org == streamif_base::beg	? SEEK_SET
				: _org == streamif_base::end 	? SEEK_END
				:								  SEEK_CUR;
		if (auto pos = ::fseek(_value, static_cast<long>(_position), org); pos >= 0)
		{
			_position	= pos;;
		}
		else
		{
			rs	= streamif_base::failbit;
		}
		return	rs;
	}

	streamif_base::iostate tell(stream_type& _value, fpos_type& _position)
	{
		_position	= ftell(_value);
		return	streamif_base::goodbit;
	}

	streamif_base::iostate size(stream_type& _value, size_type& _size)
	{
		struct stat	s;
		if (fstat(fileno(_value), &s))
		{
			return	streamif_base::failbit;
		}
		_size	= s.st_size;
		return	streamif_base::goodbit;
	}
};

using	file_streamif	= basic_streamif<FILE*>;


inline auto make_streamif(FILE* _value)
{
	return	streamif<FILE*, streamif_traits<FILE*>>(std::move(_value));
}



AMTRS_IO_NAMESPACE_END
#endif
