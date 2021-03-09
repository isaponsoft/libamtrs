/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/scriptutil.hpp>
#include <amtrs/system.hpp>
#include <stdlib.h>
#include <iostream>

#include "icpp.hpp"

using namespace amtrs;
using namespace amtrs::filesystem;
using namespace amtrs::system;


int initconfig(config& cnf, int _argc, char** _args)
{
	cnf.libamtrsDir	= getenv<std::string>("LIBAMTRS_DIR");


	int	i = 1;
	while (i < _argc)
	{
		std::string	cmd(_args[i++]);
		if (cmd == "--libamtrs-dir")
		{
			if (i == _argc)
			{
				std::cerr << "--libamtrs-dir has not directory" << std::endl;
				return	0;
			}
			cnf.libamtrsDir	= _args[i++];
			continue;
		}
		else if (cmd == "--keep-buildwork")
		{
			cnf.keepBuildWork	= true;
			continue;
		}
		else if (cmd == "--show-build-message")
		{
			cnf.showBuildMessage	= true;
			continue;
		}
		else if (cmd == "--clean-build")
		{
			cnf.cleanBuild			= true;
			continue;
		}
		else if (cmd == "--test")
		{
			cnf.testMode			= true;
			continue;
		}
		else if (cmd == "--install-libamtrs")
		{
			cnf.installAmtrs	= true;
			continue;
		}
		else
		{
			cnf.scriptPath	= std::move(cmd);
			break;
		}
	}
	while (i < _argc)
	{
		std::string	cmd(_args[i++]);
		cnf.srciptArgs += " \"";
		cnf.srciptArgs += cmd;
		cnf.srciptArgs += "\"";
	}


	if (!cnf.scriptPath.empty())
	{
		cnf.appname		= stem(cnf.scriptPath);
		cnf.scriptPath	= absdir(cnf.scriptPath);
	}

	// ------------------------------------------
	// cacheDir
	if (cnf.cacheDir.empty())
	{
		std::string	appdir("libamtrs/");
		appdir.append(cnf.appname);
		special_path<std::string>(cnf.cacheDir, specialpathtype::cache, appdir);
		cnf.cacheDir		= normalize_path(cnf.cacheDir);
		cnf.buildWorkDir	= normalize_path(cnf.cacheDir + "/build/");
	}


	// ------------------------------------------
	// libamtrsDir
	if (cnf.libamtrsDir.empty())
	{
		cnf.libamtrsDir	= getenv<std::string>("LIBAMTRS_DIR");
		if (!is_directory(cnf.libamtrsDir))
		{
			special_path<std::string>(cnf.libamtrsDir, specialpathtype::app_local, "libamtrs/libamtrs/");
		}
	}
	cnf.libamtrsDir	= absdir(cnf.libamtrsDir);
	if (cnf.testMode)
	{
		std::cout << "libamtrs-dir : " << cnf.libamtrsDir << std::endl;
	}

	if (!is_directory(cnf.libamtrsDir))
	{
		std::cerr << "libamtrs directory \"" << cnf.libamtrsDir << "\" nothing." << std::endl;
		std::cerr << "libamtrs download and install from github >  " << _args[0] << " --install-libamtrs" << std::endl;
		return	1;
	}

	if (cnf.scriptPath.empty())
	{
		return	1;
	}

	if (!is_setuped(cnf))
	{
		std::cout << "Auto setup" << std::endl;
		setup(cnf);
	}

	cnf.cmakeCmd		= ssu::add_exe("cmake")
						+ " " + cnf.cacheDir
						+ " -B \"" + cnf.buildWorkDir + "\""
						+ " \"-DCMAKE_INSTALL_PREFIX=" + cnf.cacheDir + "\""
						+ " \"-DCMAKE_BUILD_TYPE=Release\""
//						+ " \"-DCMAKE_BUILD_TYPE=Debug\""
	;

#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
//	cnf.buildCmd		= std::string("msbuild.exe ") + cnf.buildWorkDir + "\\" + cnf.appname + std::string(".vcxproj");
	cnf.buildCmd		= std::string("msbuild.exe ") + cnf.buildWorkDir + "\\INSTALL.vcxproj  /p:Configuration=Release";
	cnf.buildTargetPath	= cnf.buildWorkDir + std::string("\\Release\\") + ssu::add_exe(cnf.appname);
#else
	cnf.buildCmd		= std::string("make -C ") + cnf.buildWorkDir + " install";
	cnf.buildTargetPath	= normalize_path(cnf.buildWorkDir + "/" + ssu::add_exe(cnf.appname));
#endif
	cnf.executePath		= normalize_path(cnf.cacheDir + "/" + ssu::add_exe(cnf.appname));
	return	0;
}
