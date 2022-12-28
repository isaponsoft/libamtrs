/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__fwd__hpp
#define	__libamtrs__io__fwd__hpp
#include <ios>
#include <iosfwd>
#include <type_traits>
#include "../memory/ref_object.hpp"
AMTRS_IO_NAMESPACE_BEGIN

template<class...>
struct	reader_traits;

template<class SRC, class Traits = reader_traits<SRC>>
class	stream_reader;

template<class...>
struct	writer_traits;

template<class SRC, class Traits = writer_traits<SRC>>
class	stream_writer;


struct	reader_base : public ref_object
{
	using	size_type	= size_t;
	using	pointer		= void*;
};


struct	writer_base : public ref_object
{
	using	size_type	= size_t;
	using	pointer		= void const*;
};



template<class T>
struct	reader_traits<T*> : reader_traits<T>
{
	using	size_type	= reader_base::size_type;
	using	pointer		= reader_base::pointer;
	using	reader_type	= T*;

	size_type read(reader_type _reader, pointer _buff, size_type _len)
	{
		return	reader_traits<T>::read(*_reader, _buff, _len);
	}
};


template<class T>
struct	reader_traits<T&> : reader_traits<T>
{
	using	size_type	= reader_base::size_type;
	using	pointer		= reader_base::pointer;
	using	reader_type	= T&;

	size_type read(reader_type _reader, pointer _buff, size_type _len)
	{
		return	reader_traits<T>::read(_reader, _buff, _len);
	}
};


template<class T>
struct	writer_traits<T*> : writer_traits<T>
{
	using	size_type	= writer_base::size_type;
	using	pointer		= writer_base::pointer;
	using	writer_type	= T*;

	size_type write(writer_type _writer, pointer _buff, size_type _len)
	{
		return	writer_traits<T>::write(*_writer, _buff, _len);
	}
};


template<class T>
struct	writer_traits<T&> : writer_traits<T>
{
	using	size_type	= writer_base::size_type;
	using	pointer		= writer_base::pointer;
	using	writer_type	= T&;

	size_type write(writer_type _writer, pointer _buff, size_type _len)
	{
		return	writer_traits<T>::write(_writer, _buff, _len);
	}
};


AMTRS_IO_NAMESPACE_END
#endif
