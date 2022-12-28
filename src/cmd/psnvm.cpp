/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <amtrs/string/text.hpp>
#include <amtrs/console/arguments.hpp>
#include <amtrs/console/editline.hpp>
#include <amtrs/console/escape.hpp>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/filesystem/util.hpp>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/trnstext.hpp>
#include <amtrs/system/env.hpp>
#include <amtrs/lang/psnvm.hpp>
#include <amtrs/util/cmd/psnvm/psnvm.hpp>
using namespace AMTRS_NAMESPACE;
using namespace AMTRS_STRING_NAMESPACE;
using namespace AMTRS_SOURCECODE_NAMESPACE;
using namespace PSNVM_NAMESPACE;
using namespace AMTRS_CMD_PSNVM_NAMESPACE;
using namespace psnvm::lang;
using namespace amtrs::console;

struct	option
{
	prepro			pp;
	psn				ps;
	validator		vl;
	vmachine		vm;
	sstring			code;

	void reset();
};

void usage(text _self)
{
	std::cout << "usage)" << std::endl;
	std::cout << _self << " [optinos...] sourcefile arguments..." << std::endl;
	std::cout << _self << " [optinos...] -c outfilename sourcefiles" << std::endl;
	std::cout << _self << " [optinos...] -e sourcecodes..." << std::endl;
}

void print_error(char const* _filename, text _code, simplearray<errorinfo>& errors);
void inttruput_mode(option& opts);


int main(int _argc, char* _args[])
{
	trnstext::set_from_file("D:/proj/isaponsoft/libamtrs/share/psnvm/translate", "psnvm.txt");



	setlocale( LC_CTYPE, "" );

	view<char*>		argv(_args, static_cast<size_t>(_argc));
	text			self	= *argv++;
	option			opts;
	opts.reset();

	if (((text)argv.front_at()) == "-z")
	{
		opts.vm.memory_error	= [&](void* mem, text location, bool write) -> bool
		{
			auto	locate	= compute_location(opts.code, location.s, 1);
			std::cerr << locate.filename << "(" << locate.line << ") : warn : Unbound memory access" << std::endl;
			return	true;
		};
		++argv;
	}


	if (argv.empty())
	{
		inttruput_mode(opts);
		return	0;
	}

	// compile mode.
	if (((text)argv.front_at()) == "-c")
	{
	}
	// eval mode.
	else if (((text)argv.front_at()) == "-e")
	{
		++argv;
		sstring	src;
		while (!argv.empty())
		{
			src = src + " " + *argv++;
		}

		opts.code	= opts.pp.process(src, "commandline");
		if (opts.pp.errors.size() > 0)
		{
			AMTRS_FORCE_LOG("pp error");
			return	1;
		}

		// parse
		auto		fnode	= opts.ps.parse(((text)opts.code).trim("\r\n\t "));
		if (opts.ps.errors.size() > 0)
		{
			print_error(nullptr, opts.code, opts.ps.errors);
			return	1;
		}

		// validate
		auto		vnode	= opts.vl.validate(fnode.data(), fnode.size());
		if (opts.vl.errors.size() > 0)
		{
			print_error(nullptr, opts.code, opts.vl.errors);
			return	1;
		}

		// run
		auto		ret		= opts.vm.run(vnode);
		return	static_cast<int>(ret.value.i);
	}
	// unkown option.
	else if (((text)argv.front_at()).starts_with("-"))
	{
		usage(self);
		return	1;
	}
	// run mode.
	else
	{
		text		file	= *argv++;
		sstring		codebp;
		if (!filesystem::file_get_contents(codebp, file).good())
		{
			std::cerr << file << "not load." << std::endl;
			return	1;
		}

		opts.code	= opts.pp.process(codebp, file);
		if (opts.pp.errors.size() > 0)
		{
			AMTRS_FORCE_LOG("pp error");
			return	1;
		}

		// parse
		auto		fnode	= opts.ps.parse(((text)opts.code).trim("\r\n\t "));
		if (opts.ps.errors.size() > 0)
		{
			print_error(file.data(), opts.code, opts.ps.errors);
			return	1;
		}

		// validate
		auto		vnode	= opts.vl.validate(fnode.data(), fnode.size());
		if (opts.vl.errors.size() > 0)
		{
			print_error(file.data(), opts.code, opts.vl.errors);
			return	1;
		}

		// run
		auto		ret		= opts.vm.run(vnode);
		return	static_cast<int>(ret.value.i);
	}
	return	0;
}


void print_error(char const* _filename, text _code, simplearray<errorinfo>& errors)
{
	if (!_filename)
	{
		for (auto& e : errors)
		{
			auto	m	= _TT(error_string(e.err));
			auto	fm	= format(m.data(), {(text)e.token1, (text)e.token2});
			std::cerr << console::fgcolor_light_red << (text)c8_to_cr(fm) << console::fgcolor_reset << std::endl;
			for (auto& amb : e.ambitiouses)
			{
				std::cerr << console::fgcolor_light_magenta << (text)amb.token << console::fgcolor_reset << std::endl;
			}
		}
		return;
	}

	for (auto& e : errors)
	{
		auto	loc	= compute_location(_code, e.location.data());
		if (loc.filename.empty())	loc.filename	= _filename;
		auto	m	= _TT(error_string(e.err));
		auto	fm	= format(m.data(), {(text)e.token1, (text)e.token2});
		std::cerr << console::fgcolor_light_red << loc.filename << "(" << loc.line << ") : E" << PSNVM_ERRORCODE(e.err) << " : " << (text)c8_to_cr(fm) << console::fgcolor_reset << std::endl;
		for (auto& amb : e.ambitiouses)
		{
			auto	loc	= compute_location(_code, amb.location.data());
			if (loc.filename.empty())	loc.filename	= _filename;
			std::cerr << console::fgcolor_light_magenta << loc.filename << "(" << loc.line << ") : " << (text)amb.token << console::fgcolor_reset << std::endl;
		}
	}
}


void inttruput_mode(option& opts)
{
	editline	editor(io::fdesc::stdo(), io::fdesc::stdi());
	editor.prompt("> ");

	sstring		line;
	for (;;)
	{
		sstring			cmd;
		if (!editor.readline(cmd))
		{
			line.clear();
			editor.prompt("> ");
		}
		if (cmd.empty())
		{
			continue;
		}
		line.append(cr_to_c8(cmd));
		if (trim_blank(line) == "exit")
		{
			break;
		}
		if (trim_blank(line) == "reset")
		{
			editor.reset();
			line.clear();
			continue;
		}
		if (trim_blank(line) == "")
		{
			line.clear();
			continue;
		}

		opts.reset();
		opts.code	= std::move(line);	line.clear();
		opts.code	= opts.code + ";";
		opts.ps.errors.clear();
		auto		fnode	= opts.ps.parse(((text)opts.code).trim("\r\n\t "));
		if (opts.ps.errors.size() > 0)
		{
			print_error(nullptr, opts.code, opts.ps.errors);
			continue;
		}

		// validate
		auto		vnode	= opts.vl.validate(fnode.data(), fnode.size());
		if (opts.vl.errors.size() > 0)
		{
			print_error(nullptr, opts.code, opts.vl.errors);
			continue;
		}

		// run
		auto		ret		= opts.vm.run(vnode);
		std::cout << ret.value.i << std::endl;
		opts.reset();

		editor.prompt("> ");
	}

}


void option::reset()
{
	vl.reset();
	vm.reset();

	vl.add("echo", "void", {"int"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
	{
		std::cout << _argv[0];
		return	0;
	});
	vl.add("operator new", "void*", {"size_t"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
	{
		size_t	s	= _argv[0];
		void*	p	= operator new (s);
		vmachine::current()->add_memory(p, s);
		return	reinterpret_cast<intmax_t>(p);
	});
	vl.add("operator delete", "void", {"void*"}, [](intmax_t* _argv, size_t _argc) -> intmax_t
	{
		void*	p	= reinterpret_cast<void*>(_argv[0]);
		if (vmachine::current()->remove_memory(p))
		{
			operator delete (p);
		}
		return	0;
	});
}

