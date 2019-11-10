/*
 * Copyright (c) 2019 Isao Shibuya (isapon)
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */
#ifndef	__passion__string__h
#define	__passion__string__h
#include <stdlib.h>
#include "refcount.h"
#include "string_view.h"
#ifdef	_MSC_VER
#pragma	warning(push)
#pragma	warning(disable:4190)
#endif
PASSION_EXTERN_C_BEGIN

/*
 * passion string.
 */
struct	passion_string
{
	size_t					size;
	char					text[1];

	#ifdef	__cplusplus
	operator passion_string_view () const noexcept { return {text, size}; }
	#endif	//cplusplus
};


inline extern char* passion_string_unsafe_strcpy(char* _d, const char* _s)
{
	size_t	i	= 0;
	do
	{
		_d[i] = _s[i];
	} while (_s[i++]);
	return	_d + i - 1;
}


inline extern void passion_string_unsafe_memcpy(void* _d, const void* _s, size_t _len)
{
	size_t	i	= 0;
	do
	{
		((char*)_d)[i] = ((char*)_s)[i];
	} while (++i < _len);
}

inline extern size_t passion_string_unsafe_strlen(const char* _s)
{
	size_t	i;
	for (i = 0; _s[i]; ++i)
	{
		;
	}
	return	i;
}


inline extern void passion_string_zero_clear(void* _buff, size_t _length)
{
	size_t		i;
	size_t		s;
	s	= _length / sizeof(void*);
	if (s > 0)
	{
		for (i = 0; i < s; ++i)
		{
			((void**)_buff)[i]	= 0;
		}
		_buff	= (void**)_buff + i;
	}
	s	= _length % sizeof(void*);
	for (i = 0; i < s; ++i)
	{
		((int8_t*)_buff)[i] = 0;
	}
}


inline extern struct passion_string_view passion_string_view_create_from_passion_string(const struct passion_string* _s)
{
	struct passion_string_view	retval;
	retval._begin	= _s->text;
	retval._size	= _s->size;
	return	retval;
}


inline extern struct passion_string* passion_string_create(size_t _size)
{
	struct passion_string*	retval	= (struct passion_string*)malloc(sizeof(struct passion_string) + _size);
	retval->size	= _size;
	return	retval;
}


inline extern struct passion_string* passion_string_create_and_copy(const char* _text, size_t _size)
{
	struct passion_string*	retval	= passion_string_create(_size);
	if (retval)
	{
		passion_string_unsafe_strcpy(retval->text, _text);
	}
	return	retval;
}


inline extern void passion_string_destroy(struct passion_string* _val)
{
	if (_val)
	{
		free((void*)_val);
	}
}



PASSION_EXTERN_C_END
#ifdef	_MSC_VER
#pragma	warning(pop)
#endif
#endif
