/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__icpp__icpp__hpp
#define	__icpp__icpp__hpp
#include <string>
#include <amtrs/filesystem.hpp>
#include <amtrs/scriptutil.hpp>


struct	scriptinfo
{
	int			startline;
	std::string	includes;
};



struct	config
{
	bool		keepBuildWork		= false;
	bool		showBuildMessage	= false;
	bool		cleanBuild			= false;
	bool		testMode			= false;
	bool		installAmtrs		= false;

	std::string	appname;

	std::string	scriptPath;
	std::string	srciptArgs;
	std::string	libamtrsDir;
	std::string	cacheDir;
	std::string	installDir;
	std::string	buildWorkDir;

	std::string	cmakeCmd;
	std::string	buildCmd;
	std::string	buildTargetPath;
	std::string	executePath;
};


inline std::string absdir(std::string const& _path)
{
	return	amtrs::regex_replace_callback<std::string>(amtrs::filesystem::normalize_path(amtrs::filesystem::absolute_path(_path)), R"(\\)", [&](auto&)
	{
		return	"/";
	});
}


// arguments.cpp

int initconfig(config& cnf, int _argc, char** _args);


// build.cpp

bool exec_cmake(config& cnf);
bool exec_build(config& cnf);
amtrs::ssu::update_result update_files(config& cnf);
amtrs::ssu::update_result update_files_cmake(config& cnf);
amtrs::ssu::update_result update_files_source(config& cnf);


// setup.cpp

bool is_setuped(config& cnf);
bool setup(config& cnf);

#endif
