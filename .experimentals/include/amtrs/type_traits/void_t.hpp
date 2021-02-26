/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__type_utils__void_type__hpp
#define __libamtrs__type_utils__void_type__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(void_t)
// http://open-std.org/JTC1/SC22/WG21/docs/cwg_defects.html#1558
template<typename... T>
struct	void_t
{
	using	type	= void;
};
AMTRS_IMPLEMENTS_END(void_t)


#if 1
// ============================================================================
// 常に void
// ----------------------------------------------------------------------------
template<typename... T>
using	void_t	= typename AMTRS_IMPLEMENTS(void_t)::void_t<T...>::type;
#else
template<typename... T>
using	void_t	= void;
#endif


AMTRS_NAMESPACE_END
#endif
