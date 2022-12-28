/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <regex>
#include <amtrs/filesystem.hpp>
#include <amtrs/string.hpp>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {


function_generator	_update("update", [](status& _status, std::string& line, function_params const& params) -> bool
{
	auto	c		= amtrs::split_chars((std::string_view)params.arguments, (std::string_view)" ");
	auto	it		= c.begin();
	auto	end		= c.end();
	if (it == end)
	{
		report("uptade has not parameters.");
		return	false;
	}
	auto	destname	= (std::string)*it++;


	if (it == end)
	{
		report("uptade has not parameters.");
		return	false;
	}
	auto	srcname		= (std::string)*it++;


	if (!amtrs::filesystem::exists(srcname))
	{
		apgen::error(amtrs::format<std::string>("\"%s\" not found.", srcname.c_str()));
		return	false;
	}
	std::string	txt		= amtrs::filesystem::file_get_contents<std::string>(srcname);
	txt	= process(_status, txt, srcname);

	update(destname, txt);

	line	= "";
	return	true;
});



}
