/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <string>
#include <amtrs/filesystem/@>
#include <amtrs/string/join.hpp>
#include <amtrs/string/trim.hpp>
#include <amtrs/system/env.hpp>
#include <psnvm/.error.hpp>
#include <psnvm/.compiler-reportor.hpp>
using namespace amtrs;
PASSION_NAMESPACE_BEGIN


static char const* get_error_message(errorcode _code)
{
	switch (_code)
	{
		default:									return	u8"不明なエラーが発生しました。";
		case ok :									return	"ok";
		case pp_error :								return	u8"プリプロセッサエラー。";
		case pp_include_has_not_filename :			return	u8"includeの後ろににファイル名がありません。";
		case pp_include_unclose_filename :			return	u8"includeのファイル名が閉じられていません。";
		case pp_include_cant_open :					return	u8"指定されたincludeファイル '%s' が開けません。";
		case pp_define_has_not_name :				return	u8"#define にマクロ名が指定されていません。";
		case pp_define_unkown_name :				return	u8"#define の後ろにマクロ名として認識できない不明な文字 '%s' があります。";
		case pp_define_unclosed_parameter :			return	u8"#define %s の関数パラメータが ')' で閉じれられていません。";
		case pp_define_unkown_parameter_name :		return	u8"#define %s の関数パラメータに不明な文字 '%s' があります。";
		case pp_define_not_parameter_separator :	return	u8"#define %s の関数パラメータのセパレータ , の位置に不明な文字 '%s' があります。";
		case pp_undef_has_not_name :				return	u8"#undef にマクロ名が指定されていません。";
		case pp_undef_unkown_name :					return	u8"#undef の後ろにマクロ名として認識できない不明な文字 '%s' があります。";
		case pp_undef_nothing :						return	u8"#undef で指定するマクロ %s は未定義です。";
		case pp_ifdef_nothing_macro_name :			return	u8"#ifdef の後ろにマクロ名が記載されていません。";
		case pp_if_nothing_expression_value :		return	u8"#if/#elif の式が完結していません。";
		case pp_if_singleope_not_value :			return	u8"#%s の後ろに式または値がありません。";
		case pp_if_nothing_endif :					return	u8"#%s から始まる条件ブロックに #endif がありません。";
		case pp_if_expression_unterminate :			return	u8"#if の式が改行で終わっていません。";
		case pp_if_expression_unkown :				return	u8"#if の式に不明な \"%s\" が現れました。";
		case pp_if_expression_unkown_return :		return	u8"#if の式が完了する前に改行が現れました。";
		case pp_if_expression_unclosed_circle :		return	u8"'(' が閉じられていません。";
		case pp_if_has_not_circle :					return	u8"%s に必要な '(' がありません。";
		case pp_if_has_not_closecircle :			return	u8"%s が ')' で閉じられていません。";
		case pp_if_defined_has_not_name :			return	u8"defined にマクロ名が指定されていません。";
		case pp_include_not_include_paths :			return	u8"指定されたincludeファイル '%s' が検索パスから見つかりませんでした。";
	}
}



vmstring reportor::error::whats() const
{
	return	format<vmstring>(get_error_message(code), hints[0].c_str(), hints[1].c_str());
}


PASSION_NAMESPACE_END
