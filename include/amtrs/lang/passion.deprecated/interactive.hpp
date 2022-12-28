/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__interactive__hpp
#define	__libamtrs__passion__interactive__hpp
#include <amtrs/console/escape.hpp>
#include <amtrs/console/editline.hpp>
#include "vm.hpp"
#include "compiler.hpp"
#define	PSNVM_SLSH_NAMESPACE_BEGIN	PSNVM_NAMESPACE_BEGIN namespace slsh {
#define	PSNVM_SLSH_NAMESPACE_END	} PSNVM_NAMESPACE_END
PSNVM_SLSH_NAMESPACE_BEGIN
using namespace amtrs;
using namespace amtrs::console;

struct	intoptions
{
//	sstring												prompt			= "\x1b[32m\\u@\\h\x1b[37m:\x1b[34m\\w\x1b[37m % ";
	sstring												prompt			= "\\u@\\h:\\w % ";
	sstring												rc_file			= "~/.slshrc";			// Run controll file.
	sstring												history_file	= "~/.slsh_history";	// 履歴を保存するファイルの名称
	size_t												history_limit	= 100;					// 保存する最大件数
	typeutil::callback<lang::expression_callback>		expr_callback;				// 式のエラーを検出した時にエラーを訂正するためのコールバック
};


struct	argopt
{
	enum	modes
	{
		mode_default,
		mode_prepro,
	};

	struct	source
	{
		sstring		filename;
		sstring		txt;
		bool		module	= false;
	};

	modes						mode			= modes::mode_default;
	simplearray<sstring>		files;
	simplearray<source>			sources;
	simplearray<lang::argdef>	defines;
	unsigned int				compilerFlags	= 0;
	bool						compileOnly		= false;
	bool						shellMode		= false;
};


PSNVM_API int arguments_analyze(argopt& _opt, int _argc, char** _args);
PSNVM_API int interactive_main(int _argc, char** _args, vmstate& _vm, intoptions const& _options);
PSNVM_API int interactive_mode(vmstate& _vm, intoptions& _options);
PSNVM_API void print_error(errorhandler& _err, text _filename, char const* _source);
PSNVM_API editline* curent_editor();

PSNVM_SLSH_NAMESPACE_END
#endif
