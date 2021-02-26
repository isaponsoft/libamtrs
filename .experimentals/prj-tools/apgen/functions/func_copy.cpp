/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <filesystem>
#include <regex>
#include <amtrs/filesystem/@>
#include <amtrs/string/@>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {


function_generator	_copy("copy", [](status& _status, std::string& line, function_params const& params) -> bool
{
	auto	c		= amtrs::split_chars((std::string_view)params.arguments, (std::string_view)" ");
	auto	it		= c.begin();
	auto	end		= c.end();
	if (it == end)
	{
		report("copy has not parameters.");
		return	false;
	}
	auto	destname	= amtrs::filesystem::normalize_path((std::string)*it++);


	if (it == end)
	{
		report("copy has not parameters.");
		return	false;
	}
	std::string	srcname		= amtrs::filesystem::normalize_path((std::string)*it++);
	std::string	base		= (std::string)amtrs::filesystem::filename(srcname);


	std::string	destbase	= destname;
	destbase += "\\";
	destbase += base;
	destbase += "\\";

	for (auto& p: std::filesystem::recursive_directory_iterator(srcname))
	{
		std::string	srcfile	= amtrs::filesystem::normalize_path(p.path().string());
		if (std::filesystem::is_regular_file(srcfile))
		{
			std::string	rel			= srcfile.substr(srcname.size());
			std::string	destname	= destbase;
			destname	+= rel;
			destname	=  amtrs::filesystem::normalize_path(destname);

			auto	srcdata	= amtrs::filesystem::file_get_contents<std::string>(srcfile);
			update(destname, srcdata);
		}
	}
	line	= "";
	return	true;
});


}
