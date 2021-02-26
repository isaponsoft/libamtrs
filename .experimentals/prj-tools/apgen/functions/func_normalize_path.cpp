/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <regex>
#include <amtrs/string/@>
#include <amtrs/filesystem/@>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {


function_generator	_normalize_path("normalize_path", [](status& _status, std::string& line, function_params const& params) -> bool
{
	auto	c		= amtrs::split_chars((std::string_view)params.arguments, (std::string_view)" ");
	auto	it		= c.begin();
	auto	end		= c.end();
	if (it == end)
	{
		report("normalize_path has not parameters.");
		return	false;
	}
	std::string	path	= (std::string)*it++;


	bool	escape	= false;
	if (it != end)
	{
		if (*it == "TRUE")
		{
			escape = true;
		}
	}

	line	= amtrs::filesystem::normalize_path(path, escape);

	std::cout << "normalize path : " << path << " => " << line << std::endl;
	return	true;
});



}
