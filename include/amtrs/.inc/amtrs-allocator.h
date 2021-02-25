/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__amtrs__allocator__hpp
#define	__libamtrs__amtrs__allocator__hpp
AMTRS_EXTERN_C_BEGIN


typedef	struct
{
	void* (*allocate)(size_t s);
	void* (*reallocate)(void* mem, size_t s);
	void  (*free)(void* mem);
} amtrs_allocator;

amtrs_allocator* amtrs_allocator_get_default();


AMTRS_EXTERN_C_END
#endif
