/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__refcount__h
#define	__passion__refcount__h
#if	0
#include <assert.h>


#ifdef	_MSC_VER
#include <windows.h>
struct passion_refcount
{
	long				count;
};
#else
#include <stdatomic.h>
struct passion_refcount
{
	atomic_intmax_t		count;
};
#endif

PASSION_EXTERN_C_BEGIN
inline extern void passion_refcount_initialize(struct passion_refcount* _val)
{
	#ifdef	_MSC_VER
	_val->count	= 0;
	#else
	atomic_init(&_val->count, 0);
	#endif
}


inline extern void passion_refcount_add(struct passion_refcount* _val)
{
	#ifdef	_MSC_VER
	InterlockedIncrement(&_val->count);
	#else
	atomic_fetch_add(&_val->count, 1);
	#endif
}


inline extern passion_bool passion_refcount_release(struct passion_refcount* _val)
{
	#ifdef	_MSC_VER
	return	InterlockedDecrement(&_val->count) == 0;
	#else
	return	atomic_fetch_sub(&_val->count, 1) == 1;
	#endif
}

PASSION_EXTERN_C_END
#endif
#endif
