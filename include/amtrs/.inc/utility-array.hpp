/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__utiluty__array__hpp
#define	__libamtrs__utiluty__array__hpp
#include <array>
#include <tuple>
#include <utility>
AMTRS_NAMESPACE_BEGIN


//! std::array<T, N> のユーティリティラッパークラス。
//! 加算やT (&)[N] 型を返すオペレータなどの便利な機能を追加します。
//! std::array<T, N> の代わりとしてそのまま利用可能です。
template<class T, std::size_t N>
struct	array : std::array<T, N>
{
	using	_base_type	= std::array<T, N>;
	using	_base_type::_base_type;
	using	buffer_type	= T(*)[N];
	typedef	      T (&buff_type)[N];
	typedef	const T (&const_buff_type)[N];

	//! T[N] から初期化します。
	constexpr array(const T (&_src)[N])
	{
		std::copy(std::begin(_src), std::end(_src), _base_type::data());
	}

	//! 二つの固定長配列を繋げます。
	template<std::size_t RN>
	constexpr auto operator + (const std::array<T, RN>& _r) const noexcept -> array<T, N + RN> 
	{
		array<T, N + RN>	retval;
		retval.set_from_two_index(*this, _r, std::make_index_sequence<N>(), std::make_index_sequence<RN>());
		return	retval;
	}

	//! T(&)[N] の参照へ変換します。
	constexpr operator buff_type () noexcept
	{
		return	reinterpret_cast<buff_type>(std::array<T, N>::data());
	}

	//! T(&)[N] の参照へ変換します。
	constexpr operator const_buff_type () const noexcept
	{
		return	reinterpret_cast<const_buff_type>(std::array<T, N>::data());
	}

private:
	template<std::size_t N1, std::size_t N2, std::size_t... I1, std::size_t... I2>
	constexpr void set_from_two_index(const std::array<T, N1>& _a1, const std::array<T, N2>& _a2, std::index_sequence<I1...>, std::index_sequence<I2...>) noexcept
	{
		*this = { getchar<I1>(_a1)..., getchar<I2>(_a2)... };
	}

	template<std::size_t I, std::size_t RN>
	static constexpr char getchar(const std::array<T, RN>& _ary) noexcept { return _ary[I]; }

	template<class FT, std::size_t FN>
	friend	struct	array;
};


//! 指定する値を、配列に置き換えます。
//! 例えば、make_array((uint32_t)0x11223344) であればリトルエンディアンなら { 44, 33, 22, 11} 
//! ビッグエンディアンなら { 11, 22, 33, 44 } になります。
template<class T = uint8_t, class V>
constexpr auto make_array(V _value) -> array<T, sizeof(V)>
{
	T	buff[sizeof(V)];
	*reinterpret_cast<V*>(&buff[0]) = _value;
	return	array<T, sizeof(V)>(buff);
}

//! 指定する値を、配列に置き換えます。
//! 例えば、make_array((uint32_t)0x11223344) であればリトルエンディアンなら { 44, 33, 22, 11} 
//! ビッグエンディアンなら { 11, 22, 33, 44 } になります。
template<class T, std::size_t N>
constexpr auto make_array(const T (&_ary)[N]) -> array<T, N>
{
	return	array<T, N>(_ary);
}


AMTRS_NAMESPACE_END
#endif
