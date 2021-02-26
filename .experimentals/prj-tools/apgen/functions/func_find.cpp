/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <regex>
#include <amtrs/filesystem/@>
#include <amtrs/string/@>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {


function_generator	_find("find", [](status& _status, std::string& line, function_params const& params) -> bool
{
	auto	c		= amtrs::split_chars((std::string_view)params.arguments, (std::string_view)" ");
	auto	it		= c.begin();
	auto	end		= c.end();
	if (it == end)
	{
		report("find has not parameters.");
		return	false;
	}

	auto	name	= *it++;
	if (it == end)
	{
		report("find has not parameters.");
		return	false;
	}

	for (; it != end; ++it)
	{
		std::string	path	= std::string(*it);
		path += "/";
		path += name;
		if (amtrs::filesystem::is_relative(path))
		{
			path	= amtrs::format<std::string>("%s/%s", _status.values["APGEN_STARTUP_DIR"].c_str(), path.c_str());
		}
		path	= amtrs::filesystem::normalize_path(path);
		if (amtrs::filesystem::exists(path))
		{
			report(amtrs::format<std::string>("find match \"%s\"", path.c_str()));
			line	= *it;
			return	true;
		}
		report(amtrs::format<std::string>("find unmatch \"%s\"", path.c_str()));
	}
	line	= "";
	return	true;
});



}
