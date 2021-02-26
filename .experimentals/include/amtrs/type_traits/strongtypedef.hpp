/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_utils__strongtypedef__hpp
#define __libamtrs__type_utils__strongtypedef__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class T>
class	strongtypedef
{
public:
	using	value_type	= T;

	strongtypedef() = default;
	constexpr strongtypedef(strongtypedef const& _r) noexcept : v(_r.v) {}
	constexpr strongtypedef(value_type _v) noexcept : v(_v) {}
	constexpr strongtypedef& operator = (strongtypedef const& _r) noexcept { v = _r.v; return *this; }
	constexpr bool operator == (strongtypedef const& _r) noexcept { return v == _r.v; }
	constexpr bool operator != (strongtypedef const& _r) noexcept { return v == _r.v; }
	constexpr operator value_type () const noexcept { return v; }

private:
	value_type	v;
};

AMTRS_NAMESPACE_END
#endif
