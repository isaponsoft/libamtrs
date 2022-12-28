/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <typeinfo>
#include <typeindex>
#ifndef	__libamtrs__drvwin32__error__hpp
#define	__libamtrs__drvwin32__error__hpp

#ifndef	EBADRQC
#define	EBADRQC		56
#endif

#ifndef	ENONET
#define	ENONET		64
#endif

#ifndef	ECOMM
#define	ECOMM		70
#endif

#ifndef	ENOTUNIQ
#define	ENOTUNIQ	76
#endif

#ifndef	ELIBBAD
#define	ELIBBAD		80
#endif

#ifndef	ENOMEDIUM
#define	ENOMEDIUM	123
#endif

AMTRS_OS_NAMESPACE_BEGIN
int get_errorno(DWORD winerr) noexcept;
AMTRS_OS_NAMESPACE_END
AMTRS_NAMESPACE_BEGIN

template<>
struct	errorval_type<DWORD>
{
	size_t operator () ()
	{
		return	std::hash<std::type_index>{}(std::type_index(typeid(DWORD)));
	}
};


AMTRS_NAMESPACE_END
#endif
