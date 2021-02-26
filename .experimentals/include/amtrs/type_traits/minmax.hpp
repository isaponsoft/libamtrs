/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_utils__minmax__hpp
#define __libamtrs__type_utils__minmax__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class T>
struct	minmax
{
public:
	using	value_type	= T;

	minmax() = default;
	minmax(minmax const&) = default;
	minmax& operator = (minmax const&) = default;

	minmax(value_type _min, value_type _max)
		: minimam(_min)
		, maximam(_max)
	{}

	bool operator == (minmax const& _r) const noexcept { return minimam == _r.minimam && minimam == _r.minimam; }
	bool operator != (minmax const& _r) const noexcept { return !operator == (_r); }

	value_type	minimam;
	value_type	maximam;
};


AMTRS_NAMESPACE_END
#endif
