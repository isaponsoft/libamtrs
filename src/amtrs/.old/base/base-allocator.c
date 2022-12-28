/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <stdlib.h>

amtrs_allocator* amtrs_allocator_get_default()
{
	static	amtrs_allocator	alloc;
	static	int				init;
	if (!init)
	{
		init	= 1;
		alloc.allocate		= &malloc;
		alloc.reallocate	= &realloc;
		alloc.free			= &free;
	}
	return	&alloc;
}
