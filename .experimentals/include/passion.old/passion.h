/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__passion__h
#define	__passion__passion__h
#include <stddef.h>
#include <stdint.h>
//#include <cstddef>
//#include <amtrs/string.hpp>

#include "loader/binaryfile.h"

#if		defined(WIN32) || defined(_WIN32)
#define	PASSIONAPI
#else
#define	PASSIONAPI
#endif

#ifdef	__cplusplus
#define	passion_null			nullptr
#define	passion_true			true
#define	passion_false			false
#define	PASSION_EXTERN_C_BEGIN	extern "C" {
#define	PASSION_EXTERN_C_END	}
#else
#define	passion_null	NULL
#define	passion_true	1
#define	passion_false	0
#define	PASSION_EXTERN_C_BEGIN
#define	PASSION_EXTERN_C_END
#endif


/*
 * Prototypes.
 */
struct	passion_object;				/* Object. */
struct	passion_class_manager;		/* passion::vmclass_manager */
struct	passion_class;				/* passion::vmclass */
struct	passion_context;			/* passion::vmcontext */

typedef	intmax_t		passion_register;
typedef	uint8_t			passion_bool;


#ifdef	PASSION_VMBYTECODE_TRACE
#	define	PSNVMBC_TRACE_LOG(...)	AMTRS_DEBUG_LOG(__VA_ARGS__)
#else
#	define	PSNVMBC_TRACE_LOG(...)
#endif

#ifdef	_MSC_VER
#pragma	warning(push)
#pragma	warning(disable:4190)
#endif


#include "def.hpp"
#include "inc/context.h"
#include "inc/context.hpp"
#include "inc/function.h"
#include "inc/integer.h"
#include "inc/namespaces.h"
#include "inc/namespaces.hpp"
#include "inc/refcount.h"
#include "inc/symbole_list.h"
#include "inc/string.hpp"
#include "inc/struct.hpp"
#include "inc/struct_info.h"
#include "inc/typename_utils.h"
#include "inc/variant.h"
#include "inc/vtable.h"

#ifdef	_MSC_VER
#pragma	warning(pop)
#endif

#endif
