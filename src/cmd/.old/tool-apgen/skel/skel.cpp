/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <regex>
#include <unordered_map>
#include <apgen.hpp>

using namespace apgen;
namespace fs	= amtrs::filesystem;


class	SkeltonGenerator
		: public generator_base
{
public:
	SkeltonGenerator(option const& _opt)
	{
		using namespace amtrs;
	}

	// 引数の整合性チェック
	virtual bool arguments_check(std::string const& _key, std::string const& _value, option const& _opt) override
	{
		return	true;
	}

	virtual int execute(status const& _state, option const& _opt) override
	{
		generate_with_template("APGen.txt", "APGen.template.APGen.txt", _state, "APGen.txt");
		return	0;
	}

	void generate_with_template(std::string _target, std::string _filename, status const& _state, std::string _template)
	{
		std::string	templatename	= _state.values["APGEN_TEMPLATE_DIR"];
		templatename += "/";
		templatename += _template;
		if (!amtrs::filesystem::exists(templatename))
		{
			apgen::error(amtrs::format<std::string>("template \"%s\" not found.", templatename.c_str()));
			return;
		}

		std::string	txt;
		std::string	filename;
		if (auto it = _state.values.find(_filename); it != _state.values.end())
		{
			std::string	fname	= it->second;
			if (!amtrs::filesystem::exists(fname))
			{
				apgen::error(amtrs::format<std::string>("\"%s\" not found.", fname.c_str()));
				return;
			}
			filename		= fname;
		}
		else
		{
			filename		= templatename;
		}
		txt		= amtrs::filesystem::file_get_contents<std::string>(filename);
		txt		= process(const_cast<status&>(_state), txt, filename);
		update(_target, txt);
	}

};


apgen::autolink	_skel({"skel", [](auto& opt) -> generator_base*
{
	return	new SkeltonGenerator(opt);
}});

