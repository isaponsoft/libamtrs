/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__vm__vmvtable__h
#define	__passion__vm__vmvtable__h
#include <stdlib.h>
#include "function.h"
PASSION_EXTERN_C_BEGIN

//! Virtual table.
struct	passion_vtable
{
	size_t						base_offset;		//!< Offset size to object top;
	struct passion_function*	functions[1];		//!< Function table.		// 0 is destractor.
};


inline extern struct passion_vtable* passion_vtable_create(size_t _function_size)
{
	return	(struct passion_vtable*)malloc(
				  sizeof(struct passion_vtable)
				+ sizeof(struct passion_function*) * (_function_size - 1)
			);
}


inline extern void passion_vtable_destroy(struct passion_vtable* _vtable)
{
	if (_vtable)
	{
		free((void*)_vtable);
	}
}


PASSION_EXTERN_C_END
#endif
