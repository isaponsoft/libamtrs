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
#ifndef	__libamtrs__iostream__bin__hpp
#define	__libamtrs__iostream__bin__hpp
#include "def.hpp"
AMTRS_IOSTREAM_NAMESPACE_BEGIN


template<class...>
struct	bin;

//! std::ostream や std::istream に対し、data(), size() を持つデータ構造を
//! バイナリとして出力／入力するためのラッパーです。
template<class T>
struct	bin<T>
{
	char* data() noexcept { return reinterpret_cast<char*>(&v); }
	const char* data() const noexcept { return reinterpret_cast<const char*>(&v); }
	std::size_t size() const noexcept { return sizeof(v); }
	T&	v;
};




template<class T>
auto make_bin(T& _data) noexcept
{
	return	bin<T>{_data};
}

template<class T>
auto make_bin(const T& _data) noexcept
{
	return	bin<const T>{_data};
}


template<class T>
std::istream& operator >> (std::istream& _in, bin<T> _b)
{
	_in.read(_b.data(), _b.size());
	return	_in;
}

template<class T>
std::ostream& operator << (std::ostream& _out, const bin<T>& _b)
{
	_out.write(_b.data(), _b.size());
	return	_out;
}

AMTRS_IOSTREAM_NAMESPACE_END
#endif
