/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <regex>
#include "../apgen.hpp"
#include "functions.hpp"

namespace apgen {


function_generator	_dump("dump", [](status& _status, std::string& line, function_params const& params) -> bool
{
	// マクロの一覧を出力する
	auto	nlen	= 4;
	auto	vlen	= 5;
	for (auto& m : _status.values)
	{
		nlen	= nlen > m.first.size() ? (int)nlen : (int)m.first.size();
		vlen	= vlen > m.second.size() ? (int)vlen : (int)m.second.size();
	}
	std::string	border = "+-";
	for (int i = 0; i < nlen; ++i)
	{
		border += "-";
	}
	border += "-+-";
	for (int i = 0; i < vlen; ++i)
	{
		border += "-";
	}
	border += "-+";

	auto	fmt	= amtrs::format<std::string>("| %%-%ds | %%-%ds |", (int)nlen, (int)vlen);
	apgen::report(border);
	apgen::report(amtrs::format<std::string>(fmt.c_str(), "Name", "Value"));
	apgen::report(border);
	for (auto& m : _status.values)
	{
		apgen::report(amtrs::format<std::string>(fmt.c_str(), m.first.c_str(), m.second.c_str()));
	}
	apgen::report(border);

	line	= "";
	return	true;
});



}
