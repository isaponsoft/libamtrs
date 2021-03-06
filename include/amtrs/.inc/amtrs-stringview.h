/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__amtrs__stringview__hpp
#define	__libamtrs__amtrs__stringview__hpp
AMTRS_EXTERN_C_BEGIN

/*
 * string view
 * c/c++ compatible.
 */
typedef	struct
{
	char const*			data;
	size_t				size;
} amtrs_stringview;


AMTRS_EXTERN_C_END
#endif
