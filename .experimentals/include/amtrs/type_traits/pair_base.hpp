/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_traits__pair_base__hpp
#define __libamtrs__type_traits__pair_base__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<class First, class Second>
struct	pair_base
{
	struct	type
			: First
			, Second
	{
		type() = default;

		template<class A, class B>
		type(A&& _a, B&& _b)
			: First(_a)
			, Second(_b)
		{}
	};

	using	first_type	= First;
	using	second_type	= Second;
};


template<class First>
struct	pair_base<First, void>
{
	using	type		= First;
	using	first_type	= First;
	using	second_type	= void;
};

template<class Second>
struct	pair_base<void, Second>
{
	using	type		= Second;
	using	first_type	= void;
	using	second_type	= Second;
};

template<class First, class Second>
using	pair_base_t	= typename pair_base<First, Second>::type;



AMTRS_NAMESPACE_END
#endif
