/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <apgen.hpp>

using namespace apgen;



class	UnzipVirtualGenerator
		: public generator_base
{
public:
	// 引数の整合性チェック
	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) override
	{
		if (_key == "ZIP")
		{
			mURL		= _value;
			return	true;
		}

		return	false;
	}


	virtual int execute(status const& _state, option const& _opt) override
	{
		if (mURL.empty())
		{
			std::cerr << "ZIP nothing." << std::endl;
		}
		std::ifstream	in(mURL, std::ios::binary);
		auto			vs	= amtrs::io::make_vstreamif(in);
		unzip(vs);
		return	0;
	}


	std::string							mURL;
};


apgen::autolink	_unzip({"unzip", [](auto& opt) -> generator_base*
{
	return	new UnzipVirtualGenerator();
}});

