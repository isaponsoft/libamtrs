/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__utiluty__array__hpp
#define	__libamtrs__utiluty__array__hpp
#include <array>
#include <tuple>
#include <utility>
AMTRS_NAMESPACE_BEGIN


//! std::array<T, N> �̃��[�e�B���e�B���b�p�[�N���X�B
//! ���Z��T (&)[N] �^��Ԃ��I�y���[�^�Ȃǂ֗̕��ȋ@�\��ǉ����܂��B
//! std::array<T, N> �̑���Ƃ��Ă��̂܂ܗ��p�\�ł��B
template<class T, std::size_t N>
struct	array : std::array<T, N>
{
	using	_base_type	= std::array<T, N>;
	using	_base_type::_base_type;
	using	buffer_type	= T(*)[N];
	typedef	      T (&buff_type)[N];
	typedef	const T (&const_buff_type)[N];

	//! T[N] ���珉�������܂��B
	constexpr array(const T (&_src)[N])
	{
		std::copy(std::begin(_src), std::end(_src), _base_type::data());
	}

	//! ��̌Œ蒷�z����q���܂��B
	template<std::size_t RN>
	constexpr auto operator + (const std::array<T, RN>& _r) const noexcept -> array<T, N + RN> 
	{
		array<T, N + RN>	retval;
		retval.set_from_two_index(*this, _r, std::make_index_sequence<N>(), std::make_index_sequence<RN>());
		return	retval;
	}

	//! T(&)[N] �̎Q�Ƃ֕ϊ����܂��B
	constexpr operator buff_type () noexcept
	{
		return	reinterpret_cast<buff_type>(std::array<T, N>::data());
	}

	//! T(&)[N] �̎Q�Ƃ֕ϊ����܂��B
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


//! �w�肷��l���A�z��ɒu�������܂��B
//! �Ⴆ�΁Amake_array((uint32_t)0x11223344) �ł���΃��g���G���f�B�A���Ȃ� { 44, 33, 22, 11} 
//! �r�b�O�G���f�B�A���Ȃ� { 11, 22, 33, 44 } �ɂȂ�܂��B
template<class T = uint8_t, class V>
constexpr auto make_array(V _value) -> array<T, sizeof(V)>
{
	T	buff[sizeof(V)];
	*reinterpret_cast<V*>(&buff[0]) = _value;
	return	array<T, sizeof(V)>(buff);
}

//! �w�肷��l���A�z��ɒu�������܂��B
//! �Ⴆ�΁Amake_array((uint32_t)0x11223344) �ł���΃��g���G���f�B�A���Ȃ� { 44, 33, 22, 11} 
//! �r�b�O�G���f�B�A���Ȃ� { 11, 22, 33, 44 } �ɂȂ�܂��B
template<class T, std::size_t N>
constexpr auto make_array(const T (&_ary)[N]) -> array<T, N>
{
	return	array<T, N>(_ary);
}


AMTRS_NAMESPACE_END
#endif
