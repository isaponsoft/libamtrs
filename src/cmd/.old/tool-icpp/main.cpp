/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/scriptutil.hpp>
#include <amtrs/system.hpp>
#include <stdlib.h>
#include <iostream>

#include "icpp.hpp"

using namespace amtrs;
using namespace amtrs::filesystem;
using namespace amtrs::system;



void clean(config& cnf);

int buildrun(config& cnf);

int main(int _argc, char** _args)
{
	config	cnf;
	if (initconfig(cnf, _argc, _args))
	{
		return	255;
	}

	if (cnf.cleanBuild)
	{
		clean(cnf);
	}


	if (cnf.scriptPath.empty())
	{
		return	0;
	}

	std::string	cd	= std::filesystem::current_path().string();
	if (cnf.buildChangeDir)
	{
		try {
			std::filesystem::create_directories(cnf.buildWorkDir);
		} catch (...)
		{}
		std::filesystem::current_path(cnf.buildWorkDir);
	}

	int	r	= buildrun(cnf);
	if (cnf.buildChangeDir)
	{
		std::filesystem::current_path(cd);
	}
	if (r)
	{
		return	r;
	}

std::cout << "Execute" << cnf.executePath << std::endl;
	return	ssu::exec(cnf.executePath/* + cnf.srciptArgs*/);
}


int buildrun(config& cnf)
{
	auto	up	= update_files(cnf);
	if (ssu::is_bad(up))
	{
		return	255;
	}


	if (up == ssu::update_modify || cnf.buildAlways)
	{
		std::string	curdir	= std::filesystem::current_path().string();
		if (cnf.testMode)
		{
			std::cout << "Create build work directory \"" << cnf.buildWorkDir << "\"." << std::endl;
		}
		else
		{
			std::filesystem::create_directories(cnf.buildWorkDir);
			if (exists(cnf.executePath))
			{
				std::filesystem::remove(cnf.executePath);
			}
		}

#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
		std::vector<std::pair<std::string, std::string>>	envs;
		ssu::enumenvs(envs, R"("C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64)", ssu::add_bat(normalize_path(cnf.cacheDir + "/enumenv")));
		for (auto& e : envs)
		{
			SetEnvironmentVariableA(e.first.c_str(), e.second.c_str());
		}
#endif


		if (!exec_cmake(cnf))
		{
			return	255;
		}
		if (!exec_build(cnf))
		{
			return	255;
		}
		if (!cnf.keepBuildWork)
		{
			if (cnf.testMode)
			{
				std::cout << "Remove build work directory \"" << cnf.buildWorkDir << "\"." << std::endl;
			}
			else
			{
				try
				{
					std::filesystem::remove_all(cnf.buildWorkDir);
				}
				catch (...)
				{
					std::cerr << "failure remove build work directory \"" << cnf.buildWorkDir << "\"." << std::endl;
					return	255;
				}
			}
		}
	}
	return	0;
}



void clean(config& cnf)
{
	try
	{
		std::filesystem::remove_all(cnf.buildWorkDir);
	}
	catch (...)
	{
	}
	try
	{
		std::filesystem::remove_all(cnf.cacheDir);
	}
	catch (...)
	{
	}
}
