/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__preprocessor__hpp
#define	__psnvm__preprocessor__hpp
#include <list>
#include <vector>
#include <unordered_map>
#include ".compiler-codereader.hpp"
#include ".compiler-reportor.hpp"
PASSION_NAMESPACE_BEGIN namespace preprocessor {

struct	macro
{
	vmstring							filename;
	int									line;
	amtrs::shared_buffer<vmstring>		params;
	bool								has_args	= false;
	vmstring							text;
};


struct	setting
{
	struct	macrooverload
	{
		std::vector<macro>	macros;
		bool				has_noarg	= false;
		bool				has_args	= false;
	};

	std::unordered_map<vmstring, macrooverload>		defines;
	std::vector<vmstring>							include_paths;

	// ========================================================================
	//! プリプロセッサに通すソースコード
	// ------------------------------------------------------------------------
	//! 複数のソースコードを指定した場合、それらのソースコードは連結されたもの
	//! として扱います。
	//! 例えば { file1, file2 } とあった場合、file2 の先頭で include file1 を
	//! したのと同じように扱われます。
	//! 暗黙的にincludeさせたいヘッダファイルを使うときなどに有効です。
	// ------------------------------------------------------------------------
	std::vector<codereader>							sources;

	// ========================================================================
	//! 引数無しのマクロを追加します。
	// ------------------------------------------------------------------------
	void define(vmstring _name, vmstring _text, vmstring _filename = {}, int _line = -1);


	// ========================================================================
	//! マクロを追加します。
	// ------------------------------------------------------------------------
	void define(vmstring _name, macro _mcr);
};



//! プリプロセッサを実行します
vmstring preprocess(reportor* _report, setting const& _setting);




} PASSION_NAMESPACE_END
#endif
