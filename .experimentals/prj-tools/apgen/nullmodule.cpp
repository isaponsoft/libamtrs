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
#include <amtrs/filesystem/@>
#include <amtrs/filesystem/zip/@>
#include <amtrs/string/@>
#include <amtrs/string/regex/@>
#include <amtrs/process/@>
#include <amtrs/opt/io/zlib-stream_in.hpp>

#include <apgen.hpp>

using namespace apgen;
namespace fs	= amtrs::filesystem;


class	NullModuleGenerator
		: public generator_base
{
public:
	NullModuleGenerator(option const& _opt)
	{
	}

	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) override
	{
		return	true;
	}

	virtual int execute(status const& _status, option const& _opt) override
	{
		return	0;
	}
};


apgen::autolink	_nullModule({"null", [](auto& opt) -> generator_base*
{
	return	new NullModuleGenerator(opt);
}});
