#ifndef	__libamtrs__string__string__h
#define	__libamtrs__string__string__h
#include <stdlib.h>
#include "string_view.h"
#ifdef	_MSC_VER
#pragma warning(push)
#pragma warning(disable:4190)
#endif
AMTRS_EXTERN_C_BEGIN

/*
 * passion string.
 */
typedef	struct	_amtrs_string
{
	size_t					size;
	char					text[1];

	#ifdef	__cplusplus
	operator amtrs_string_view () const noexcept { return {text, size}; }
	#endif	//cplusplus
} amtrs_string;


inline extern amtrs_string_view amtrs_string_to_string_view(amtrs_string* _string)
{
	amtrs_string_view	retval;
	if (_string)
	{
		retval.txt	= _string->text;
		retval.len	= _string->size;
	}
	else
	{
		retval.txt	= amtrs_null;
		retval.len	= 0;
	}
	return	retval;
}


inline extern void amtrs_string_zero_clear(void* _buff, size_t _length)
{
	memset(_buff, 0, _length);
}


inline extern amtrs_string_view amtrs_string_view_create_from_amtrs_string(const amtrs_string* _s)
{
	amtrs_string_view	retval;
	retval.txt	= _s->text;
	retval.len	= _s->size;
	return	retval;
}


inline extern amtrs_string* amtrs_string_create(size_t len)
{
	amtrs_string*	retval	= (amtrs_string*)malloc(sizeof(amtrs_string) + len);
	retval->size	= len;
	return	retval;
}


inline extern amtrs_string* amtrs_string_join(amtrs_string_view _a, amtrs_string_view _b)
{
	amtrs_string*	retval	= amtrs_string_create(_a.len + _b.len);
	if (retval)
	{
		char*					cursor	= (char*)retval->text;
		if (_a.len > 0)
		{
			memcpy(cursor, _a.txt, _a.len);
			cursor += _a.len;
		}
		if (_b.len > 0)
		{
			memcpy(cursor, _b.txt, _b.len);
			cursor += _b.len;
		}
		*cursor	= '\0';
	}
	return	retval;
}



inline extern amtrs_string* amtrs_string_create_and_copy(const char* _text, size_t len)
{
	amtrs_string*	retval	= amtrs_string_create(len);
	if (retval)
	{
		memcpy(retval->text, _text, len);
		retval->text[len] = '\0';
	}
	return	retval;
}


inline extern amtrs_string* amtrs_string_create_from_string_view(amtrs_string_view _text)
{
	return	amtrs_string_create_and_copy(_text.txt, _text.len);
}


inline extern amtrs_string* amtrs_string_duplicate(amtrs_string* _source)
{
	if (!_source)
	{
		return	amtrs_null;
	}
	return	amtrs_string_create_and_copy(_source->text, _source->size);
}


inline extern void amtrs_string_destroy(amtrs_string* _val)
{
	if (_val)
	{
		free((void*)_val);
	}
}



AMTRS_EXTERN_C_END
#ifdef	_MSC_VER
#pragma warning(pop)
#endif
#endif
