/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__lexer__hpp
#define	__libamtrs__passion__lexer__hpp
#include "../../memory/simplearray.hpp"
#include "../../string/sourcecode.hpp"
#include "../../string/text.hpp"
#include "core-error.hpp"
PSNVM_LANG_NAMESPACE_BEGIN
using namespace AMTRS_NAMESPACE;
using namespace AMTRS_SOURCECODE_NAMESPACE;
using	textlist	= simplearray<text>;

class	compiler;
struct	codefunction;
struct	exprnode;
struct	exprstatus;


// 関数シグネチャ
struct	function_signature
{
	text		type;			// 戻り値の型
	textlist	parameters;		// パラメータの型
};






// 型名と思われる文字列を読み取ります。
readdata<text> read_typename(text _code);


// 関数宣言時のパラメータリストを読み取ります。
//	_parameterlist	()を取り除いたパラメータリストを示すソースコード。
bool read_parameterlist(parameterlist& _result, compiler& _compiler, text _parameterlist);


// リスト
// , で区切られたテキストを分離します。
text read_textlist(textlist& _result, errorhandler& _error, text _code);


/*
 * ; で完結する式を抜き出します。
 * ; が無い場合は nullptr を返します。
 */
char const* read_expression(text& _result, compiler& _compiler, text _code);


// シグネチャを解析します。
readdata<function_signature> read_signature(errorhandler& _err, text _code);

exprnode pass2_readvalue(exprstatus& _state, compiler& _compiler, codefunction* _function, operator_type _prev);


/*
 * ()や{}、[]で囲まれたブロックを抽出します。
 * 最初の位置は開くブラケットである必要があります。
 * ネストされたブラケットと文字列の判定を行います。
 */
char const* read_brackes_block(text& _result, text _code, errorhandler& _eh);
text read_brackes_block(errorhandler& _eh, text _code);



PSNVM_LANG_NAMESPACE_END
#endif
