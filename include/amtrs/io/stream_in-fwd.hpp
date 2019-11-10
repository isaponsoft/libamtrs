/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__io__stream_in_fwd__hpp
#define	__libamtrs__io__stream_in_fwd__hpp
#include <iosfwd>
#include "def.hpp"
#include "io_traits-fwd.hpp"
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
