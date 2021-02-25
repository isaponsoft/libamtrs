/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__endian__hpp
#define	__libamtrs__memory__endian__hpp
AMTRS_NAMESPACE_BEGIN

//! c++20 �� STL �����p�\�ȏꍇ�� std::endian �ւ̃G�C���A�X�ɂȂ�܂��B
enum class endian
{
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};


namespace endian_util {

//! �������ɏ������񂾎��ɃG���f�B�A��������ւ��悤�ɒl��ύX���܂��B
template<class T>
constexpr T bitswap(T _value) noexcept
{
	if constexpr (sizeof(T) > 1)
	{
		constexpr std::size_t	length	= sizeof(T);
		constexpr std::size_t	count	= length / 2;
		constexpr std::size_t	last	= length - 1;
		T	retval(_value);
		for (std::size_t i = 0; i < count; ++i)
		{
			std::swap(reinterpret_cast<char*>(&retval)[i], reinterpret_cast<char*>(&retval)[last - i]);
		}
		return	retval;
	}
	else
	{
		return	_value;
	}
}


//! �������ɏ������񂾎��ɃG���f�B�A��������ւ��悤�ɒl��ύX���܂��B
template<class V, class T>
constexpr V bitswap(T const (&_value)[sizeof(V)]) noexcept
{
	return	bitswap(*reinterpret_cast<const V*>(_value));
}


//! �������ɏ������񂾎��Ɏw�肵���G���f�B�A���ɂȂ�悤�ɒl��ύX���܂��B
template<endian E, class V, class T>
constexpr V bitswap_if(T const (&_value)[sizeof(V)]) noexcept
{
	if constexpr (endian::native != E)	return	bitswap<V>(_value);
	else								return	*reinterpret_cast<const V*>(_value);
}

//! �������ɏ������񂾎��Ɏw�肵���G���f�B�A���ɂȂ�悤�ɒl��ύX���܂��B
template<endian E, class V>
constexpr V bitswap_if(V _value) noexcept
{
	if constexpr (endian::native != E)	return	bitswap(_value);
	else								return	_value;
}

//! �������ɏ������񂾎��Ɏw�肵���G���f�B�A���ɂȂ�悤�ɒl��ύX���܂��B
template<endian E, class V, class T>
constexpr V encode(T const (&_value)[sizeof(V)]) noexcept { return bitswap_if<E, V>(_value); }

//! �w�肵���G���f�B�A���ŏ������܂ꂽ����������ǂݍ��񂾃f�[�^���l�C�e�B�u�̒l�Ƃ��Ďg����悤�ɒl���C�����܂��B
template<endian E, class V, class T>
constexpr V decode(T const (&_value)[sizeof(V)]) noexcept { return bitswap_if<E, V>(_value); }

//! �������ɏ������񂾎��Ɏw�肵���G���f�B�A���ɂȂ�悤�ɒl��ύX���܂��B
template<endian E, class T>
constexpr T encode(T _value) noexcept { return bitswap_if<E>(_value); }

//! �w�肵���G���f�B�A���ŏ������܂ꂽ����������ǂݍ��񂾃f�[�^���l�C�e�B�u�̒l�Ƃ��Ďg����悤�ɒl���C�����܂��B
template<endian E, class V>
constexpr V decode(V _value) noexcept { return bitswap_if<E>(_value); }



} // endian_util
AMTRS_NAMESPACE_END
#endif
