/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__compiler__compiler__hpp
#define	__psnvm__compiler__compiler__hpp
#include ".vm-vm.hpp"
#include ".compiler-codereader.hpp"
PASSION_NAMESPACE_BEGIN
class	prepro;


class	compiler
{
public:
	// ==============================================================
	//! コンパイラを生成します。
	// --------------------------------------------------------------
	//! コンパイル結果を格納する有効なVMを指定します。
	// --------------------------------------------------------------
	compiler(vm* _vm)
		: mVM(_vm)
	{}

	// ==============================================================
	//! コンパイルを行います。
	// --------------------------------------------------------------
	//! 結果は errors() などのメンバ関数で確認できます。
	// --------------------------------------------------------------
	void compile(codereader _source);


protected:
	vm*			mVM;
};




PASSION_NAMESPACE_END
#endif
