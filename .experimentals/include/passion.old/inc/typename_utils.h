/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__typename_utils__h
#define	__passion__typename_utils__h
#include <string.h>
#ifdef	_MSC_VER
#pragma	warning(push)
#pragma	warning(disable:4190)
#endif
PASSION_EXTERN_C_BEGIN


// void
passion_bool				passion_is_void(amtrs_string_view _typename);

// integer
passion_bool				passion_is_integer(amtrs_string_view _typename);
passion_bool				passion_size_is_addressable(amtrs_string_view _typename);

// float and double.
size_t						is_floating_point(amtrs_string_view _typename);

// array
passion_bool 				passion_is_array(amtrs_string_view _typename);
size_t 						passion_compute_array_dimention(amtrs_string_view _typename);
amtrs_string_view			passion_trim_array_last_dimention(amtrs_string_view _typename);
amtrs_string_view			passion_trim_array_dimentions(amtrs_string_view _typename);

// gc pointer
passion_bool				passion_is_gcpointer(amtrs_string_view _typename);
amtrs_string_view			passion_trim_gcpointer(amtrs_string_view _typename);

// pointer
passion_bool				passion_is_pointer(amtrs_string_view _typename);
amtrs_string_view			passion_trim_pointer(amtrs_string_view _typename);

// pointer & gc pointer
passion_bool				passion_is_pointerable(amtrs_string_view _typename);

// gc pointer, array, string
passion_bool				passion_is_gcpointerable(amtrs_string_view _typename);


// const
amtrs_string_view			passion_trim_const(amtrs_string_view _from);


// reference
passion_bool				passion_is_reference(amtrs_string_view _typename);
amtrs_string_view			passion_trim_reference(amtrs_string_view _typename);


// typename
amtrs_string_view			passion_get_typename(amtrs_string_view _typename);
amtrs_string_view			passion_get_typename_and_modifier(amtrs_string_view _typename);


// function
amtrs_string_view			passion_get_functionpointer_result(amtrs_string_view _typename);
passion_bool				passion_is_function(struct passion_functiontype_info* _buffer, amtrs_string_view _typename);
passion_bool				passion_is_functionpointer(struct passion_functiontype_info* _buffer, amtrs_string_view _typename);
passion_bool				passion_parse_functiontype(struct passion_functiontype_info* _buffer, amtrs_string_view _typename);
void						passion_split_parameter_types(amtrs_string_view* _buffer, struct passion_functiontype_info* _info);

// primitive type
size_t						passion_size_of_primitive(amtrs_string_view _typename);
size_t						passion_size_of_without_structure(amtrs_string_view _typename);


/*
 * passion typename utility.
 */
#define	PASSION_TYPENAME_DIMENTION_SUFFIX		"[]"
#define	PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE	2

#define	PASSION_TYPENAME_GCPOINTER_SUFFIX		"^"
#define	PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE	1

#define	PASSION_TYPENAME_POINTER_SUFFIX			"*"
#define	PASSION_TYPENAME_POINTER_SUFFIX_SIZE	1

#define	PASSION_TYPENAME_REFERENCE_SUFFIX		"&"
#define	PASSION_TYPENAME_REFERENCE_SUFFIX_SIZE	1

#define	PASSION_TYPENAME_PRIMITIVE_INT			"int"
#define	PASSION_TYPENAME_PRIMITIVE_CHAR			"char"
#define	PASSION_TYPENAME_PRIMITIVE_BOOL			"bool"
#define	PASSION_TYPENAME_PRIMITIVE_INT8_T		"int8_t"
#define	PASSION_TYPENAME_PRIMITIVE_INT16_T		"int16_t"
#define	PASSION_TYPENAME_PRIMITIVE_INT32_T		"int32_t"
#define	PASSION_TYPENAME_PRIMITIVE_INT64_T		"int64_t"
#define	PASSION_TYPENAME_PRIMITIVE_FLOAT		"float"
#define	PASSION_TYPENAME_PRIMITIVE_DOUBLE		"double"



inline extern passion_bool passion_is_void(amtrs_string_view _typename)
{
	return	amtrs_string_view_equals(_typename, amtrs_string_view_make("void"));
}


inline extern passion_bool passion_is_integer(amtrs_string_view _typename)
{
	return	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT))
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_CHAR))
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_BOOL))
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT8_T))
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT16_T))
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT32_T))
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT64_T))
		;
}


/* ==================================================================
 * Address compatible type.
 * ------------------------------------------------------------------
 * Size is cpu architecture depenences types.
 * "int" is 32bit or 64bit. Future >= 128bit...
 * pointer, gc pointer, array, string, ... is compatible "int".
 * ==================================================================*/
inline extern passion_bool passion_size_is_addressable(amtrs_string_view _typename)
{
	return	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT))
		||	passion_is_pointerable(_typename)
		||	passion_is_gcpointerable(_typename);
}


/* ==================================================================
 * Check floating point type.
 * ------------------------------------------------------------------
 * return
 *	Not floating type is return 0.
 *	Floating type is size of bytes. If "float" is 4, "double" is 8.
 * ==================================================================*/
inline extern size_t is_floating_point(amtrs_string_view _typename)
{
	return	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_FLOAT))	? sizeof(float)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_DOUBLE))	? sizeof(double)
		:	0;
}


/****************************************************************************
 *
 * Array
 *
 ****************************************************************************/

inline extern passion_bool passion_is_array(amtrs_string_view _typename)
{
	if (_typename.len < PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE)
	{
		return	passion_false;
	}
	if (!memcmp(_typename.txt + _typename.len - PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE, PASSION_TYPENAME_DIMENTION_SUFFIX, PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE))
	{
		return	passion_true;
	}
	return	passion_false;
}


/* ==================================================================
 * Compute array dimention.
 * ------------------------------------------------------------------
 * If typename isn't array, return 0.
 * ==================================================================*/
inline extern size_t passion_compute_array_dimention(amtrs_string_view _typename)
{
	const char*	cursor;
	size_t		count;

	if (_typename.len < PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE)
	{
		return	0;
	}

	count	= 0;
	cursor	= _typename.txt + _typename.len - PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE;
	while (!memcmp(cursor, PASSION_TYPENAME_DIMENTION_SUFFIX, PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE))
	{
		++count;
		cursor -= PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE;
		if (cursor < _typename.txt)
		{
			break;
		}
	}
	return	count;
}



/* ==================================================================
 * Trim last dimmention.
 * ==================================================================*/
inline extern amtrs_string_view passion_trim_array_last_dimention(amtrs_string_view _typename)
{
	const char*	cursor;

	if (_typename.len < PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE)
	{
		return	_typename;
	}

	cursor	= _typename.txt + _typename.len - PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE;
	if (!memcmp(cursor, PASSION_TYPENAME_DIMENTION_SUFFIX, PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE))
	{
		_typename.len	-= 2;
	}
	return	_typename;
}



/* ==================================================================
 * Trim all dimmentions.
 * ==================================================================*/
inline extern amtrs_string_view passion_trim_array_dimentions(amtrs_string_view _typename)
{
	const char*	cursor;

	if (_typename.len < PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE)
	{
		return	_typename;
	}

	cursor	= _typename.txt + _typename.len - PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE;
	while (!memcmp(cursor, PASSION_TYPENAME_DIMENTION_SUFFIX, PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE))
	{
		_typename.len	-= 2;
		cursor			-= PASSION_TYPENAME_DIMENTION_SUFFIX_SIZE;
		if (cursor < _typename.txt)
		{
			break;
		}
	}
	return	_typename;
}


/****************************************************************************
 *
 * gc pointer
 *
 ****************************************************************************/

/* ==================================================================
 * ==================================================================*/
inline extern passion_bool passion_is_gcpointer(amtrs_string_view _typename)
{
	if (_typename.len < PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE)
	{
		return	passion_false;
	}
	return	(0 == memcmp(&_typename.txt[_typename.len - PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE], PASSION_TYPENAME_GCPOINTER_SUFFIX, PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE))
			? passion_true
			: passion_false;
}



/* ==================================================================
 * ==================================================================*/
inline extern amtrs_string_view passion_trim_gcpointer(amtrs_string_view _typename)
{
	if (passion_is_gcpointer(_typename))
	{
		_typename.len -= PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE;
	}
	return	_typename;
}



/****************************************************************************
 *
 * pointer
 *
 ****************************************************************************/


inline extern passion_bool passion_is_pointer(amtrs_string_view _typename)
{
	if (_typename.len < PASSION_TYPENAME_POINTER_SUFFIX_SIZE)
	{
		return	passion_false;
	}
	return	(0 == memcmp(&_typename.txt[_typename.len - PASSION_TYPENAME_POINTER_SUFFIX_SIZE], PASSION_TYPENAME_POINTER_SUFFIX, PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE))
			? passion_true
			: passion_false;
}


inline extern amtrs_string_view passion_trim_pointer(amtrs_string_view _typename)
{
	if (passion_is_pointer(_typename))
	{
		_typename.len -= PASSION_TYPENAME_POINTER_SUFFIX_SIZE;
	}
	return	_typename;
}


inline extern passion_bool passion_is_pointerable(amtrs_string_view _typename)
{
	return	passion_is_pointer(_typename) || passion_is_gcpointer(_typename);
}

inline extern passion_bool passion_is_gcpointerable(amtrs_string_view _typename)
{
	return	passion_is_gcpointer(_typename)
		||	passion_is_array(_typename)
		||	amtrs_string_view_equals(_typename, amtrs_string_view_make("string"));
}



/****************************************************************************
 *
 * const
 *
 ****************************************************************************/
inline extern amtrs_string_view passion_trim_const(amtrs_string_view _from)
{
	amtrs_string_view	sfx(" const");
	return	amtrs_string_view_ends_with(_from, sfx)
			? _from.substr(0, _from.len - sfx.len)
			: _from;
}



/****************************************************************************
 *
 * reference
 *
 ****************************************************************************/


inline extern passion_bool passion_is_reference(amtrs_string_view _typename)
{
	if (_typename.len < PASSION_TYPENAME_REFERENCE_SUFFIX_SIZE)
	{
		return	passion_false;
	}
	return	(0 == memcmp(&_typename.txt[_typename.len - PASSION_TYPENAME_REFERENCE_SUFFIX_SIZE], PASSION_TYPENAME_REFERENCE_SUFFIX, PASSION_TYPENAME_GCPOINTER_SUFFIX_SIZE))
			? passion_true
			: passion_false;
}


inline extern amtrs_string_view passion_trim_reference(amtrs_string_view _typename)
{
	if (passion_is_reference(_typename))
	{
		_typename.len -= PASSION_TYPENAME_REFERENCE_SUFFIX_SIZE;
	}
	return	_typename;
}


inline extern amtrs_string_view passion_get_typename(amtrs_string_view _typename)
{
	const char*					cursor;
	const char*					end;
	amtrs_string_view	retval;
	retval.txt	= _typename.txt;
	retval.len	= 0;
	if (_typename.len < 1)
	{
		return	retval;
	}

	cursor	= _typename.txt;
	end		= _typename.txt + _typename.len;
	if (!((*cursor >= 'A' && *cursor <= 'Z')
	  ||  (*cursor >= 'a' && *cursor <= 'z')
	  ||  *cursor == '_'
	))
	{
		return	retval;
	}
	++cursor;
	++retval.len;

	while (cursor != end)
	{
		if (!((*cursor >= 'A' && *cursor <= 'Z')
		  ||  (*cursor >= 'a' && *cursor <= 'z')
		  ||  (*cursor >= '0' && *cursor <= '9')
		  ||  *cursor == '_'
		))
		{
			if ((cursor+2 > end) || (cursor[0] != ':') || (cursor[1] != ':'))
			{
				break;
			}
			cursor += 1;
			++retval.len;
		}
		++cursor;
		++retval.len;
	}

	return	retval;
}


inline extern amtrs_string_view passion_get_typename_and_modifier(amtrs_string_view _typename)
{
	amtrs_string_view	retval	= passion_get_typename(_typename);
	const char*			cursor	= retval.txt + retval.len;
	const char*			end		= _typename.txt + _typename.len;
	if (cursor < end)
	{
		while ((cursor < end) && (*cursor == '*' || *cursor == '^'))
		{
			++cursor;
		}
		while ((cursor < end) && (*cursor == '&'))
		{
			++cursor;
		}
		retval.len	= cursor - retval.txt;
	}
	return	retval;
}


inline extern amtrs_string_view passion_get_functionpointer_result(amtrs_string_view _typename)
{
	return	passion_get_typename_and_modifier(_typename);
}


inline extern passion_bool passion_is_function(struct passion_functiontype_info* _buffer, amtrs_string_view _typename)
{
	struct passion_functiontype_info	t;
	const char*							cursor;
	const char*							end;
	amtrs_string_view					typedata;
	if (!_buffer)
	{
		_buffer	= &t;
	}

	_buffer->result_type.txt	= passion_null;
	_buffer->result_type.len	= 0;
	_buffer->parameter_size		= 0;
	_buffer->parameter.txt	= passion_null;
	_buffer->parameter.len	= 0;

	// 戻り値の型を取得
	typedata	= passion_get_functionpointer_result(_typename);
	if (typedata.len == 0)
	{
		return	passion_false;
	}
	_buffer->result_type	= typedata;

	cursor	= typedata.txt + typedata.len;
	end		= _typename.txt + _typename.len;
	if ((end - cursor) < 2)
	{
		return	passion_false;
	}
	if (*cursor++ != '(')
	{
		return	passion_false;
	}

	_buffer->parameter.txt	= cursor;
	if (*cursor == ')')
	{
		return	passion_true;
	}

	// parameter list
	while (cursor != end)
	{
		typedata.txt	= cursor;
		typedata.len	= end - cursor;
		typedata	= passion_get_typename_and_modifier(typedata);
		if (typedata.len == 0)
		{
			return	passion_false;
		}
		cursor	+= typedata.len;

		++_buffer->parameter_size;
		_buffer->parameter.len	= cursor - _buffer->parameter.txt;

		if ((end - cursor) == 0)
		{
			return	passion_false;
		}

		if (*cursor == ')')
		{
			return	passion_true;
		}
		if (*cursor != ',')
		{
			return	passion_false;
		}

		++cursor;
	}
	return	passion_false;
}


inline extern passion_bool passion_is_functionpointer(struct passion_functiontype_info* _buffer, amtrs_string_view _typename)
{
	const char*					cursor;
	const char*					end;
	amtrs_string_view	typedata;

	_buffer->result_type.txt	= passion_null;
	_buffer->result_type.len	= 0;
	_buffer->parameter_size		= 0;
	_buffer->parameter.txt	= passion_null;
	_buffer->parameter.len	= 0;

	// 戻り値の型を取得
	typedata	= passion_get_functionpointer_result(_typename);
	if (typedata.len == 0)
	{
		return	passion_false;
	}
	_buffer->result_type	= typedata;


	// (*)
	cursor	= typedata.txt + typedata.len;
	end		= _typename.txt + _typename.len;
	if ((end - cursor) < 2)
	{
		return	passion_false;
	}
	if (cursor[0] != '(')
	{
		return	passion_false;
	}
	cursor += 4;

	_buffer->parameter.txt	= cursor;
	if (*cursor == ')')
	{
		return	passion_true;
	}

	// parameter list
	while (cursor != end)
	{
		typedata.txt	= cursor;
		typedata.len	= end - cursor;
		typedata	= passion_get_typename(typedata);
		if (typedata.len == 0)
		{
			return	passion_false;
		}
		cursor	+= typedata.len;

		++_buffer->parameter_size;
		_buffer->parameter.len	= cursor - _buffer->parameter.txt;

		if ((end - cursor) == 0)
		{
			return	passion_false;
		}

		if (*cursor == ')')
		{
			return	passion_true;
		}
		if (*cursor != ',')
		{
			return	passion_false;
		}

		++cursor;
	}
	return	passion_false;
}


inline extern passion_bool passion_parse_functiontype(struct passion_functiontype_info* _buffer, amtrs_string_view _typename)
{
	const char*					cursor;
	const char*					end;
	amtrs_string_view	typedata;

	_buffer->result_type.txt	= passion_null;
	_buffer->result_type.len	= 0;
	_buffer->parameter_size		= 0;
	_buffer->parameter.txt	= passion_null;
	_buffer->parameter.len	= 0;

	// 戻り値の型を取得
	typedata	= passion_get_functionpointer_result(_typename);
	if (typedata.len == 0)
	{
		return	passion_false;
	}
	_buffer->result_type	= typedata;


	// (*)
	cursor	= typedata.txt + typedata.len;
	end		= _typename.txt + _typename.len;
	if ((end - cursor) < 2)
	{
		return	passion_false;
	}
	if (cursor[0] != '(')		// parameter list begin.
	{
		return	passion_false;
	}
	cursor += 1;

	_buffer->parameter.txt	= cursor;
	if (*cursor == ')')
	{
		return	passion_true;
	}

	// parameter list
	while (cursor != end)
	{
		typedata.txt	= cursor;
		typedata.len	= end - cursor;
		typedata	= passion_get_typename_and_modifier(typedata);
		if (typedata.len == 0)
		{
			return	passion_false;
		}
		cursor	+= typedata.len;

		++_buffer->parameter_size;
		_buffer->parameter.len	= cursor - _buffer->parameter.txt;

		if ((end - cursor) == 0)
		{
			return	passion_false;
		}

		if (*cursor == ')')
		{
			return	passion_true;
		}
		if (*cursor != ',')
		{
			return	passion_false;
		}

		++cursor;
	}
	return	passion_false;
}


inline extern void passion_split_parameter_types(amtrs_string_view* _buffer, struct passion_functiontype_info* _info)
{
	const char*					cursor	= _info->parameter.txt;
	const char*					end		= cursor + _info->parameter.len;
	amtrs_string_view*	dest	= _buffer;

	while (cursor != end)
	{
		dest->txt	= cursor;
		dest->len		= end - cursor;
		*dest			= passion_get_typename_and_modifier(*dest);
		cursor			= cursor + dest->len;
		if (cursor == end)
		{
			break;
		}
		++dest;
		++cursor;
	}
}


/* ==================================================================
 * primitive type size.
 * ------------------------------------------------------------------
 * If typename isn't primitive, return 0.
 * ==================================================================*/
inline extern size_t passion_size_of_primitive(amtrs_string_view _typename)
{
	return	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT))		? sizeof(intmax_t)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_CHAR))	? sizeof(char)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_BOOL))	? sizeof(int8_t)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT8_T))	? sizeof(int8_t)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT16_T))	? sizeof(int16_t)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT32_T))	? sizeof(int32_t)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_INT64_T))	? sizeof(int64_t)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_FLOAT))	? sizeof(float)
		:	amtrs_string_view_equals(_typename, amtrs_string_view_make(PASSION_TYPENAME_PRIMITIVE_DOUBLE))	? sizeof(double)
		:	0;
}


/* ==================================================================
 * primitive type and base type size.
 * ------------------------------------------------------------------
 * If typename is structure, return 0.
 * ==================================================================*/
inline extern size_t passion_size_of_without_structure(amtrs_string_view _typename)
{
	return	passion_is_gcpointerable(_typename)	? sizeof(void*)
		:	passion_is_pointer(_typename)		? sizeof(void*)
		:	passion_is_reference(_typename)		? sizeof(void*)
		:	passion_size_of_primitive(_typename);
}


PASSION_EXTERN_C_END
#ifdef	_MSC_VER
#pragma	warning(pop)
#endif
#endif
