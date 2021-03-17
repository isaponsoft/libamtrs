/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/scriptutil.hpp>
#include <amtrs/system.hpp>
#include <stdlib.h>
#include <iostream>

#include "icpp.hpp"
#include "icpp-wrap.hpp"

using namespace amtrs;
using namespace amtrs::filesystem;
using namespace amtrs::system;

ssu::update_result update_files(config& cnf);
ssu::update_result update_files_cmake(config& cnf);
ssu::update_result update_files_source(config& cnf);



bool exec_cmake(config& cnf)
{
	std::string	cmd	= cnf.cmakeCmd;
	if (cnf.testMode)
	{
		std::cout << "Run cmake : " << cmd << std::endl;
		return	true;
	}

	if (cnf.showBuildMessage)
	{
		if (ssu::exec(cmd))
		{
			return	false;
		}
	}
	else
	{
		std::string	cout;
		std::string	cerr;
		if (ssu::exec(cout, cerr, cmd))
		{
			std::cerr << "cmake failed." << std::endl;
			std::cerr << cout;
			return	false;
		}
	}
	return	true;
}

bool exec_build(config& cnf)
{
	std::string	cmd	= cnf.buildCmd;
	if (cnf.testMode)
	{
		std::cout << "Run build : " << cmd << std::endl;
		return	true;
	}


	if (cnf.showBuildMessage)
	{
		if (ssu::exec(cmd))
		{
			return	false;
		}
	}
	else
	{
		std::string	cout;
		std::string	cerr;
		if (ssu::exec(cout, cerr, cmd))
		{
			std::cerr << "build failed." << std::endl;
			std::cerr << cout;
			return	false;
		}
	}
/*	try
	{
		std::filesystem::copy(cnf.buildTargetPath, cnf.executePath);
	}
	catch (...)
	{
		std::cerr << "copy build target failed." << std::endl;
		return	false;
	}
*/	return	true;
}
ssu::update_result update_files(config& cnf)
{
	bool	modified	= false;

	// CMakeListsの更新確認
	auto	cmk	= update_files_cmake(cnf);
	if (is_bad(cmk))
	{
		return	ssu::update_err;
	}
	if (cmk == ssu::update_modify)
	{
		modified	= true;
	}

	// ソースコードの更新確認
	auto	src	= update_files_source(cnf);
	if (is_bad(src))
	{
		return	ssu::update_err;
	}
	if (src == ssu::update_modify)
	{
		modified	= true;
	}

	// 実行ファイルが無いならビルド
	if (!exists(cnf.executePath))
	{
		modified	= true;
	}

	// 全部更新無し
	return	modified
			? ssu::update_modify
			: ssu::update_skip;
}


ssu::update_result update_files_cmake(config& cnf)
{
	auto	cmaketxt	= regex_replace_callback<std::string>(gCMakeListsTxt, "##([^#]*)##", [&](auto& m) -> std::string
	{
		if (m[1].str() == "PRJNAME")
		{
			return	cnf.appname;
		}
		if (m[1].str() == "SRCDIR")
		{
			return	filesystem::to_slashpath(filesystem::parent_path(cnf.scriptPath));
		}
		if (m[1].str() == "LIAMTRSDIR")
		{
			return	filesystem::to_slashpath(filesystem::parent_path(cnf.libamtrsDir));
		}
		return	m[0].str();
	});

	auto	f	= normalize_path(cnf.cacheDir + "/CMakeLists.txt");
	auto	up	= ssu::update(f, cmaketxt, cnf.testMode);
	if (cnf.testMode)
	{
		if (up == ssu::update_modify)
		{
			std::cout << "Update cmake file : \"" << f << "\"." << std::endl;
		}
		if (up == ssu::update_skip)
		{
			std::cout << "No update cmake file : \"" << f << "\"." << std::endl;
		}
	}
	return	up;
}

ssu::update_result update_files_source(config& cnf)
{
	std::string	outdata(gMainCpp);
	std::string	includelist;

	int		scriptline	= 1;
	auto	scriptdata	= file_get_contents<std::string>(normalize_path(cnf.scriptPath));
	if (starts_with(scriptdata, "#!"))
	{
		auto	beg	= scriptdata.begin();
		auto	end	= scriptdata.end();
		while (beg != end && (*beg != '\n' && *beg != '\r'))
		{
			++beg;
		}
		if (beg != end)
		{
			auto	c	= *beg++;
			if ((*beg == '\n' && *beg == '\r') && (c != *beg))
			{
				++beg;
			}
		}
		++scriptline;
		scriptdata	= scriptdata.substr(beg - scriptdata.begin());
	}


	std::string	cppdata;
	cppdata	= format<std::string>("#line %d \"", (int)scriptline);
	cppdata += filesystem::to_slashpath(cnf.scriptPath);
	cppdata += "\"\n";
	cppdata += scriptdata;

	auto	f1	= normalize_path(cnf.cacheDir + "/main.cpp");
	auto	f2	= normalize_path(cnf.cacheDir + "/script.hpp");
	auto	up1	= ssu::update(f1, outdata, cnf.testMode);
	auto	up2	= ssu::update(f2, cppdata, cnf.testMode);
	if (cnf.testMode)
	{
		if (up2 == ssu::update_modify)
		{
			std::cout << "Update source file : \"" << f2 << "\"." << std::endl;
		}
		if (up2 == ssu::update_skip)
		{
			std::cout << "No update source file : \"" << f2 << "\"." << std::endl;
		}
	}
	return	up2;
}
