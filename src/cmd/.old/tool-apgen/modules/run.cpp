/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <regex>
#include <unordered_map>
#include <amtrs/amtrs.hpp>
#include <amtrs/console.hpp>
#include <amtrs/filesystem.hpp>
#include <amtrs/string.hpp>
#include <amtrs/system.hpp>
#include <apgen.hpp>

using namespace apgen;
using namespace amtrs;
using		ll	= console::loglevel;
namespace	fs	= amtrs::filesystem;


class	Runner
		: public generator_base
{
public:
	Runner(option const& _opt)
		: mOptions(&_opt)
	{}

	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) override
	{
		return	true;
	}

	virtual int execute(status const& _state, option const& _opt) override
	{
		std::string	fname	= _opt.genFile;
		std::string	txt		= amtrs::filesystem::file_get_contents<std::string>(fname);
		std::string	out		= process(const_cast<status&>(_state), fname, txt);
		if (!out.empty())
		{
			std::cout << out;
		}
		return	0;
	}

protected:
	option const*	mOptions;
};



apgen::autolink	_run({"run", [](auto& opt) -> generator_base*
{
	return	new Runner(opt);
}});

