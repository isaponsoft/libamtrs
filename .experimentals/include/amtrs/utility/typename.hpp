/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__utility__def__hpp
#define	__libamtrs__utility__def__hpp
#include "def.hpp"
#if	__has_include(<cxxabi.h>)
#include <cxxabi.h>
AMTRS_NAMESPACE_BEGIN
template<class T>
char const* typename_c_str()
{
	int		status;
	return	abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
}
AMTRS_NAMESPACE_END
#else
AMTRS_NAMESPACE_BEGIN
template<class T>
char const* typename_c_str()
{
	return	typeid(T).name();
}
AMTRS_NAMESPACE_END
#endif
#endif
