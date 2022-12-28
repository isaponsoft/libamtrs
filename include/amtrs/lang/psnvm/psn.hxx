/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__psnvm__psnparser__hxx
#define	__libamtrs__lang__psnvm__psnparser__hxx
#include "pflnode.hxx"
#include "pvlnode.hxx"
PSNVM_LANG_NAMESPACE_BEGIN


struct	sourcemode
{
	bool	document_mode	= false;
};


class	psn
{
public:
	sourcemode	mode;


	// 構文を解析し、ノードに変換します。
	//	あくまでも文法解析のみを行うため、ノードの整合性はそれほど厳密には調べません。
	auto parse(text _code) -> simplearray<pflnode>;

	// 構文の中でも１つの式を解析し、解析可能な範囲をノードにします。
	auto parse_expr(text& _code) -> pflnode;

	// 動的に生成されたコード
	//	パースされた pflnode に使用されている文字列なので pflnode を破棄するまでは残しておくこと
	simplearray<sstring>	dynamics;

	// 報告されたエラー
	simplearray<errorinfo>	errors;
};



struct	pfl
{
	bool	humam	= false;

	void serialize(pflnode& _node, typeutil::callback<void(text)> _callback);
	auto deserialize(text _src) -> pflnode;

	// 報告されたエラー
	simplearray<errorinfo>	errors;
};



struct	pvl
{
	bool	humam	= false;

	void serialize(pvlnode& _node, typeutil::callback<void(text)> _callback);
	auto deserialize(text _src) -> pvlnode;

	// 報告されたエラー
	simplearray<errorinfo>	errors;
};


PSNVM_LANG_NAMESPACE_END
#endif
