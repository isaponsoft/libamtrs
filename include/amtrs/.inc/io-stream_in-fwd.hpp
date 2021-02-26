/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream_in_fwd__hpp
#define	__libamtrs__io__stream_in_fwd__hpp
AMTRS_IO_NAMESPACE_BEGIN


// ****************************************************************************
//! 様々なオブジェクトに対して、std::istream に近いインターフェースを提供する。
// ----------------------------------------------------------------------------
//! あくまでもインターフェースのエミュレートのみを行い、内部的にはほぼ何もしない。
//! そのため、コンパイラの最適化によってオーバーヘッドはほぼ存在しないはずである。
//! 利用できるのは read(), gcount(), seekg(), tellg(), ステータスの取得系のみ。
// ----------------------------------------------------------------------------
template<class...>
class	stream_in;



template<class... T>
auto make_stream_in(stream_in<T...>& _in) -> stream_in<T...>&
{
	return	_in;
}


template<class... T>
auto make_stream_in(stream_in<T...>&& _in) -> stream_in<T...>
{
	return	_in;
}


template<class... T>
struct	io_traits<stream_in<T...>>
{
	using	io_type		= stream_in<T...>;
	using	char_type	= typename io_type::char_type;
	using	off_type	= typename io_type::off_type;
	using	size_type	= typename io_type::off_type;
	using	fpos_type	= typename io_type::pos_type;

	size_type read(void* _buffer, io_type& _io, size_type _request)
	{
		_io.read((char_type*)_buffer, _request);
		return	_io.gcount();
	}

	size_type write(io_type& _io, const char_type* _buffer, size_type _request)
	{
		return	_io.write(_io, (const char_type*)_buffer, _request);
	}

	fpos_type seek(io_type& _io, off_type _position, std::ios::seekdir _org)
	{
		return	_io.lseek(_position, _org);
	}

	size_type size(io_type& _io)
	{
		return	_io.size();
	}
};



AMTRS_IO_NAMESPACE_END
#endif
