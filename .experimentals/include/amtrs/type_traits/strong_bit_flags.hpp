/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__type_utils__strong_bit_flags__hpp
#define	__libamtrs__type_utils__strong_bit_flags__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
// enum で宣言されたビットフラグに対して強い型付けを行います。
// ----------------------------------------------------------------------------
// 次のような enum で宣言されたビットフラグがあったとします。
//
//	enum class	my_bit_flags
//	{
//		bit1		= 0x00001,
//		bit2		= 0x00002,
//		bit3		= 0x00004,
//	};
//
//	これを格納するフラグが uint32_t だとした場合。普通に書くと以下のようになります。
//
//	uint32_t	flags	= my_bit_flags::bit1 | my_bit_flags::bit2;
//
//
//
//	strong_bit_flags を使うと次のようになります。
//
//	strong_bit_flags<uint32_t, my_bit_flags>	flags{ my_bit_flags::bit1, my_bit_flags::bit2 };
//
//	もしくは次のようにも記述できます。
//
//	auto	flags	= strong_bit_flags<uint32_t, my_bit_flags>(my_bit_flags::bit1) | my_bit_flags::bit2;
//
// ----------------------------------------------------------------------------
template<class FlagsType, class BitEnumType>
struct	strong_bit_flags
{
	using flags_type			= FlagsType;
	using bits_enum_type		= BitEnumType;

	flags_type	value;

	constexpr strong_bit_flags()
		: value(0)
	{
	}

	constexpr explicit strong_bit_flags(const flags_type& _flags)
		: value(_flags)
	{
	}

	constexpr strong_bit_flags(const bits_enum_type _bits)
		: value(static_cast<flags_type>(_bits))
	{
	}

	constexpr strong_bit_flags(const strong_bit_flags& _src)
		: value(_src.value)
	{
	}

	constexpr strong_bit_flags(const std::initializer_list<bits_enum_type> _bits)
		: value(to_value(_bits))
	{
	}

	constexpr strong_bit_flags operator & (const bits_enum_type _f) const noexcept
	{
		return	strong_bit_flags(value & static_cast<flags_type>(_f));
	}

	constexpr strong_bit_flags operator & (const strong_bit_flags& _f) const noexcept
	{
		return	strong_bit_flags(value & _f.value);
	}

	constexpr strong_bit_flags operator | (const bits_enum_type _f) const noexcept
	{
		return	strong_bit_flags(value | static_cast<flags_type>(_f));
	}

	constexpr strong_bit_flags operator | (const strong_bit_flags& _f) const noexcept
	{
		return	strong_bit_flags(value | _f.value);
	}

	constexpr strong_bit_flags& operator &= (const bits_enum_type _f) const noexcept
	{
		value &= static_cast<flags_type>(_f);
		return	*this;
	}

	constexpr strong_bit_flags& operator &= (const strong_bit_flags& _f) const noexcept
	{
		value &= _f.value;
		return	*this;
	}

	constexpr strong_bit_flags& operator |= (const bits_enum_type _f) const noexcept
	{
		value |= static_cast<flags_type>(_f);
		return	*this;
	}

	constexpr strong_bit_flags& operator |= (const strong_bit_flags& _f) const noexcept
	{
		value |= _f.value;
		return	*this;
	}

	strong_bit_flags& operator = (bits_enum_type _f) noexcept
	{
		value	= _f;
		return	*this;
	}

	strong_bit_flags& operator = (const strong_bit_flags& _f) noexcept
	{
		value	= _f.value;
		return	*this;
	}

	constexpr operator const flags_type & () const noexcept
	{
		return	value;
	}

	strong_bit_flags& clear() noexcept
	{
		value	= 0;
		return	*this;
	}

	constexpr static flags_type to_value(const std::initializer_list<bits_enum_type>& _bits)
	{
		flags_type	v = 0;
		for (auto& bit : _bits)
		{
			v |= static_cast<flags_type>(bit);
		}
		return	v;
	}
};



AMTRS_NAMESPACE_END
#endif
