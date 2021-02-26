/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__error__hpp
#define	__psnvm__error__hpp
#include <amtrs/amtrs.hpp>
#include "def.hpp"
PASSION_NAMESPACE_BEGIN


// ============================================================================
//! エラーコード
// ----------------------------------------------------------------------------
enum	errorcode
{
	ok			= 0,


	// プリプロセッサ
	pp_error							= -10000,
	pp_include_has_not_filename			= pp_error - 1,		// include にファイル名が指定されていない
	pp_include_unclose_filename			= pp_error - 2,		// include <> が閉じられていない
	pp_include_cant_open				= pp_error - 3,		// include ファイルが見つからない
	pp_define_has_not_name				= pp_error - 4,		// define に名前がない
	pp_define_unkown_name				= pp_error - 5,		// define の後ろに名前として認識できない文字列がある
	pp_define_unclosed_parameter		= pp_error - 6,		// define の関数マクロの()が閉じられていない
	pp_define_unkown_parameter_name		= pp_error - 7,		// define の関数マクロ名に不明な文字がある
	pp_define_not_parameter_separator	= pp_error - 8,		// define の関数マクロパラメータに,がない
	pp_undef_has_not_name				= pp_error - 9,		// undef に名前がない
	pp_undef_unkown_name				= pp_error - 10,	// undef の後ろに名前として認識できない文字列がある
	pp_undef_nothing					= pp_error - 11,	// undef で指定するマクロは未定義です
	pp_ifdef_nothing_macro_name			= pp_error - 12,	// ifdef にマクロ名が指定されていない
	pp_if_singleope_not_value			= pp_error - 13,	// 単項演算子の後ろに値または式がない
	pp_if_nothing_expression_value		= pp_error - 14,	// if/elifの式が完結していない
	pp_if_nothing_endif					= pp_error - 15,	// endif が見つからない
	pp_if_expression_unterminate		= pp_error - 16,	// if の式が改行で終わっていない
	pp_if_expression_unkown				= pp_error - 17,	// if の式が改行で終わっていない
	pp_if_expression_unkown_return		= pp_error - 18,	// if の式が完了する前に改行がある
	pp_if_expression_unclosed_circle	= pp_error - 19,	// if の ( が閉じられていない
	pp_if_has_not_circle				= pp_error - 20,	// 関数 %s の後ろに ( がない
	pp_if_has_not_closecircle			= pp_error - 21,	// 関数 %s が ) で閉じられていない
	pp_if_defined_has_not_name			= pp_error - 22,	// の後ろに ( がない
	pp_include_not_include_paths		= pp_error - 33,	// include ファイルが検索パスから見つからない
};


PASSION_NAMESPACE_END
#endif
