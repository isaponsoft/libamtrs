/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__struct_info__h
#define	__passion__struct_info__h
#include "string.h"
PASSION_EXTERN_C_BEGIN


/*
 * 変数のオフセット位置
 */
typedef	struct	_passion_variable_info
		{
			amtrs_string*				type;
			amtrs_string*				name;
			passion_variable_size		offset;

			#ifdef	__cplusplus
			size_t get_offset() const noexcept { return passion_variable_size_compute(offset); }
			#endif
		} passion_variable_info;



/*
 * 構造体のメンバ変数情報
 */
typedef	struct	_passion_struct_variables
		{
			size_t					variable_size;			// メンバ変数の数
			passion_variable_info	variables[1];			// メンバ変数のアドレステーブル
		} passion_struct_variables;




/*
 * 指定する数のメンバ変数を格納可能なバッファを作成する。
 */
inline extern passion_struct_variables* passion_struct_variables_create(size_t _variable_size)
{
	size_t						totalSize	= sizeof(passion_struct_variables) + (sizeof(passion_variable_info) * ((int)_variable_size - 1));
	passion_struct_variables*	thiz		= (passion_struct_variables*)malloc(totalSize);
	if (!thiz)
	{
		return	passion_null;
	}
	amtrs_string_zero_clear(thiz, totalSize);
	thiz->variable_size		= _variable_size;

	return	thiz;
}


inline extern void passion_struct_variables_destroy(passion_struct_variables* _val)
{
	if (_val)
	{
		for (size_t i = 0; i < _val->variable_size; ++i)
		{
			passion_variable_info*	info	= &_val->variables[i];
			if (info->type)
			{
				amtrs_string_destroy(info->type);
			}
			if (info->name)
			{
				amtrs_string_destroy(info->name);
			}
		}
		free((void*)_val);
	}
}


PASSION_EXTERN_C_END
#endif
