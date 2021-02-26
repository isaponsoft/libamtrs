﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__iostream__bin__hpp
#define	__libamtrs__iostream__bin__hpp
AMTRS_IOSTREAM_NAMESPACE_BEGIN


template<class...>
struct	bin;

//! std::ostream �� std::istream �ɑ΂��Adata(), size() �����f�[�^�\����
//! �o�C�i���Ƃ��ďo�́^���͂��邽�߂̃��b�p�[�ł��B
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

template<class T, class Traits>
std::basic_istream<T, Traits>& operator >> (std::basic_istream<T, Traits>& _in, bin<T> _b)
{
	_in.read(_b.data(), _b.size());
	return	_in;
}

template<class T, class Traits>
std::basic_ostream<T, Traits>& operator << (std::basic_ostream<T, Traits>& _out, const bin<T>& _b)
{
	_out.write(_b.data(), _b.size());
	return	_out;
}

AMTRS_IOSTREAM_NAMESPACE_END
#endif
