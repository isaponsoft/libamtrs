/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__function__h
#define	__passion__function__h
#ifdef	_MSC_VER
#pragma	warning(push)
#pragma	warning(disable:4190)
#endif
#include "../../amtrs/string/@"
PASSION_EXTERN_C_BEGIN
struct	passion_function;
struct	passion_context;

enum passion_function_attributes
{
	virtual_attribute		= 1<<0,				//!< 仮想関数
	static_attribute		= 1<<1,				//!< 静的関数（thisを持たない）
	private_attribute		= 1<<2,				//!< private（同一クラスからのみ呼び出せる）
	protected_attribute		= 1<<3,				//!< protected（同一クラスもしくは派生クラスからのみ呼び出せる）
	public_attribute		= 1<<4,				//!< private（同一クラスからのみ呼び出せる）
	constexpr_attribute		= 1<<5,				//!< コンパイル時計算が可能
	constractor_attribute	= 1<<6,				//!< コンストラクタ
	destractor_attribute	= 1<<7,				//!< デスストラクタ
	varidic_attribute		= 1<<8				//!< 可変長さ引数を持っている
};



typedef	int (PASSIONAPI * passion_native_function)(struct passion_context* _context, void* _data);


//! 関数型の型情報
struct	passion_functiontype_info
{
	amtrs_string_view	result_type;
	size_t				parameter_size;
	amtrs_string_view	parameter;
};



/*
 *
 */
struct	passion_function
{
	passion_native_function		function;
	void*						data;
	uint32_t					attributes;

	amtrs_string_view	symbol;
	amtrs_string_view	return_type;
	amtrs_string_view	signature;
	amtrs_string_view	class_name;
	amtrs_string_view	function_name;
	size_t				argument_size;
	amtrs_string_view	arguments[1];
};


inline extern size_t passion_function_compute_symbol_length(amtrs_string_view _return_type, amtrs_string_view _class_name, amtrs_string_view _function_name, const amtrs_string_view* _argument_names, size_t _argument_size)
{
	size_t		length	= 0;
	size_t		i;
	if (_class_name.len > 0)
	{
		length	+= _class_name.len + 1;		// '+1' is '@'.
	}
	length	+= _function_name.len;
	length	+= 2 + (_argument_size >= 2 ? _argument_size - 1 : 0);		// '(' & ')' and ','
	if (_argument_size >= 1)
	{
		for (i = 0; i < _argument_size; ++i)
		{
			length += _argument_names[i].len;
		}
	}
	if (_return_type.len > 0)
	{
		// constractor and destractor has not return value.
		length	+= _return_type.len;
	}
	return	length;
}

inline extern struct passion_function* passion_function_create(amtrs_string_view _return_type, amtrs_string_view _class_name, amtrs_string_view _function_name, const amtrs_string_view* _argument_names, size_t _argument_size)
{
	size_t						i;
	size_t						symbolLength		= passion_function_compute_symbol_length(_return_type, _class_name, _function_name, _argument_names, _argument_size) + 100;
	size_t						totalLength			= sizeof(struct passion_function) + (sizeof(amtrs_string_view) * (_argument_size)) + symbolLength;
	struct passion_function*	retval				= (struct passion_function*)malloc(totalLength);
	char*						symbolTop;
	char*						text;
	char*						beg;
	if (!retval)
	{
		return	passion_null;
	}

	// シンボルを転送しながらViewを生成する
	retval->argument_size	= _argument_size;
	symbolTop				= (char*)((intptr_t)&retval->arguments[0] + sizeof(amtrs_string_view) * (_argument_size));
	text					= symbolTop;

	retval->symbol.txt			= beg = text;

	// クラス名を転送
	retval->class_name.txt		= text;
	retval->class_name.len		= 0;
	if (_class_name.len > 0)
	{
		memcpy(text, _class_name.txt, _class_name.len);
		text	+= _class_name.len;
		retval->class_name.len		= text - retval->class_name.txt;
		*text++	= '@';
	}
	// 関数名を転送
	beg								= text;
	retval->function_name.txt	= beg;
	memcpy(text, _function_name.txt, _function_name.len);
	text	+= _function_name.len;
	retval->function_name.len		= text - beg;

	retval->signature.txt		= text;
	*text++	= '(';
	if (_argument_size >= 1)
	{
		for (i = 0; i < _argument_size; ++i)
		{
			retval->arguments[i].txt	= text;
			memcpy(text, _argument_names[i].txt, _argument_names[i].len);
			text						= text + _argument_names[i].len;
			retval->arguments[i].len	= text - retval->arguments[i].txt;
			*text++	= ',';
		}
		--text;
	}
	*text++	= ')';

	// 戻り値を転送
	retval->return_type.txt	= beg = text;
	retval->return_type.len	= 0;
	if (_return_type.len > 0)
	{
		memcpy(text, _return_type.txt, _return_type.len);
		text						+= _return_type.len;
		retval->return_type.len	= text - beg;
	}
	retval->signature.len		= text - retval->signature.txt;

	retval->symbol.len		= text - retval->symbol.txt;

	*text	= '\0';
	retval->function	= passion_null;
	retval->data		= passion_null;
	retval->attributes	= 0;
	return	retval;
}



inline extern void passion_function_destroy(struct passion_function* _func)
{
	if (_func)
	{
		free((void*)_func);
	}
}


/*
 * 戻り値の型の範囲を調べ返します。
 */
inline extern amtrs_string_view passion_function_get_return_type(amtrs_string_view _symbol)
{
	const char*					cur;
	const char*					end;
	amtrs_string_view	retval	= { passion_null, 0 };

	// 戻り値を抜き出す
	cur	= _symbol.txt;
	end	= _symbol.txt + _symbol.len;
	while (cur != end && *cur != ')')
	{
		++cur;
	}
	if (cur == end)
	{
		return	retval;
	}
	retval.txt	= cur + 1;
	retval.len	= _symbol.len - (cur - _symbol.txt);
	return	retval;
}


/*
 * クラス名の範囲を調べ返します。
 */
inline extern amtrs_string_view passion_function_get_class_name(amtrs_string_view _function_symbol)
{
	const char*			mark	= amtrs_string_view_rfind(_function_symbol, amtrs_string_view_make("@"));
	amtrs_string_view	retval;
	retval.txt		= nullptr;
	retval.len		= 0;
	if (!mark)
	{
		return	retval;
	}
	retval.txt	= _function_symbol.txt;
	retval.len	= mark - _function_symbol.txt;
	return	retval;
}


PASSION_EXTERN_C_END
#ifdef	_MSC_VER
#pragma	warning(pop)
#endif
#endif
