/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <amtrs/filesystem/@>
#include <amtrs/net/@>
#include <amtrs/net/http/@>
#include "apgen.hpp"



bool apgen::update(std::string const& _target, std::string const& _data)
{
	if (amtrs::filesystem::exists(_target))
	{
		auto	now	= amtrs::filesystem::file_get_contents<std::string>(_target);
		if (now == _data)
		{
			std::cout << "Update[Skip] : " << _target << std::endl;
			return	true;
		}
	}
	if (auto dir = amtrs::filesystem::parent_path(_target); !dir.empty())
	{
		std::cout << "mkdir : " << dir << std::endl;
		amtrs::filesystem::create_directory((std::string_view)dir);
	}
	amtrs::filesystem::file_put_contents(_target, _data);
	std::cout << "Update : " << _target << std::endl;
	return	true;
}

