#ifndef	__libamtrs__iostream__bin__hpp
#define	__libamtrs__iostream__bin__hpp
#include <iostream>
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
