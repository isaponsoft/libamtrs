/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <amtrs/console.hpp>
#include "apgen.hpp"
using namespace amtrs;
using		ll	= console::loglevel;


bool apgen::update(std::string const& _target, std::string const& _data)
{
	if (amtrs::filesystem::exists(_target))
	{
		auto	now	= amtrs::filesystem::file_get_contents<std::string>(_target);
		if (now == _data)
		{
			console::log(ll::ll_info, "skip  : %s.", _target.c_str());
			return	true;
		}
	}
	if (auto dir = amtrs::filesystem::parent_path(_target); !dir.empty())
	{
		console::log(ll::ll_debug, "mkdir : %s.", std::string(dir).c_str());
		amtrs::filesystem::create_directory((std::string_view)dir);
	}
	amtrs::filesystem::file_put_contents(_target, _data);
	console::log(ll::ll_info, "update: %s.", _target.c_str());
	return	true;
}

