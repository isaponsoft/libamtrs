/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/console/arguments.hpp>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/lang/passion.hpp>

#include "slsh/slsh-cd.cxx"
#include "slsh/slsh-find.cxx"
#include "slsh/slsh-grep.cxx"
#include "slsh/slsh-history.cxx"
#include "slsh/slsh-ls.cxx"
#include "slsh/slsh-pwd.cxx"
#include "slsh/slsh-which.cxx"

using namespace AMTRS_STRING_NAMESPACE;
using namespace AMTRS_SOURCECODE_NAMESPACE;

int main(int _argc, char* _args[])
{
	using namespace amtrs::console;
	using namespace amtrs::strutil;
	using namespace psnvm;
	using namespace psnvm::slsh;

	// シェルモード用組み込みコマンド
	vmstate				vm;
	vm.installcore();
	vm.functions.add(vmni::makefunction("_history",	"int(string&)", &f_history));
	vm.functions.add(vmni::makefunction("_ls",		"int(string&)", &f_ls));
	vm.functions.add(vmni::makefunction("_which",	"int(string&)", &f_which));
	vm.functions.add(vmni::makefunction("_find",	"int(string&)", &f_find));
	vm.functions.add(vmni::makefunction("_grep",	"int(string&)", &f_grep));
	vm.functions.add(vmni::makefunction("_cd",		"int(string&)", &f_cd));
	vm.functions.add(vmni::makefunction("_pwd",		"int(string&)", &f_pwd));

	slsh::intoptions	opt;
	opt.expr_callback	=  [](lang::exprstatus& _state, lang::compiler& _compiler, lang::codefunction* _function, text& _code) -> lang::exprnode
	{
		// 改行コードが見つかるまでを抜き出す
		text	line(_code);
		auto	pos	= line.find_first_of("\n");
		if (pos != text::npos)
		{
			line	= line.substr(0, pos);
			_code	= _code.substr(pos);
		}
		else
		{
			line	= _code;
			_code	= _code.substr(_code.e);
		}

		// コマンドラインを分解 &&, ||, |, >, < があればコマンドを分離
		sstring		script;
		sstring		cmd;
		for (auto& a : parse_command_args(line))
		{
			if (a == "&&" || a == "||")
			{
				text	tkn		= read_identify_token(cmd);
				if (tkn == "which" || tkn == "ls" || tkn == "grep" || tkn == "find" || tkn == "cd" || tkn == "pwd" || tkn == "history")
				{
					sstring	args		= encode_escape_string(trim_blank(text(cmd).substr(tkn.e)));
					script.append(format("(_{}(\"{}\") == 0)", { tkn, args }));
				}
				else
				{
					script.append(format("(exec(\"{}\") == 0)", { encode_escape_string(cmd) }));
				}
				if (a == "&&")
				{
					script.append(" && ");
				}
				if (a == "||")
				{
					script.append(" || ");
				}
				cmd.clear();
				continue;
			}
			if (!cmd.empty())
			{
				cmd.append(" ");
			}
			cmd.append(a);
		}
		if (!cmd.empty())
		{
			text	tkn		= read_identify_token(cmd);
			if (tkn == "which" || tkn == "ls" || tkn == "grep" || tkn == "find" || tkn == "cd" || tkn == "pwd" || tkn == "history")
			{
				sstring	args		= encode_escape_string(trim_blank(text(cmd).substr(tkn.e)));
				script.append(format("(_{}(\"{}\") == 0)", { tkn, args }));
			}
			else
			{
				script.append(format("(exec(\"{}\") == 0)", { encode_escape_string(cmd) }));
			}
		}
		script.append(";");

		auto	e	= lang::parse_expression(_state, _compiler, _function, script);
		for (auto& e : _compiler.errors)
		{
			io::fdesc::stdo() << e.message() << "\n";
		}
		return	e;
	};
	auto	exitcode	= 	slsh::interactive_main(_argc, _args, vm, opt);
	return	exitcode;
}
