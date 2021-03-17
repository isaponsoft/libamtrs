/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/optparse.hpp>
#include <amtrs/scriptutil.hpp>
#include <amtrs/system.hpp>
#include <stdlib.h>
#include <iostream>

#include "icpp.hpp"

using namespace amtrs;
using namespace amtrs::filesystem;
using namespace amtrs::system;

struct	visualstudio
{
	static constexpr auto vswhere() { return R"(C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe)"; }
	static constexpr auto vcvarsall() { return R"(VC\Auxiliary\Build\vcvarsall.bat)"; }


	std::string	vspath;
	std::string	vcvars;
	std::string	build;
	std::string	cmake;

	bool check_installed(std::string _cachedir)
	{
		build	= ssu::which("MSbuild.exe");
		cmake	= ssu::which("cmake.exe");
		if (build.empty() || cmake.empty())
		{
			if (!exec_vcvars(_cachedir))
			{
				return	false;
			}
			build	= ssu::which("MSbuild.exe");
			cmake	= ssu::which("cmake.exe");
		}
//		return	!build.empty() && !cmake.empty();
return	false;
	}




private:
	bool exec_vcvars(std::string _cachedir)
	{
		std::string	out;
		std::string	err;
		std::string	bf	= normalize_path(_cachedir + "/vcenv.bat"); 
		if (!exists(bf))
		{
			if (!exists(vswhere()))
			{
				return	false;
			}

			// vswhere.exe を実行して Visual Studio のインストールディレクトリを探す
			if (ssu::exec(out, err, format<std::string>("\"%s\"", vswhere())))
			{
				return	false;
			}
			regex_match_callback(out, R"(installationPath: +(.*)[\r\n])", [&](auto& m)
			{
				vspath	= m[1].str();
			});
			if (vspath.empty())
			{
				return	false;
			}

			// vcvarscall.bat を探して実行する
			vcvars	= normalize_path(vspath + "/" + vcvarsall());
			if (!exists(vcvars))
			{
				return	false;
			}

			std::string	bat = "@ECHO OFF\r\n";
			bat.append(format<std::string>("CALL \"%s\" x64\r\n", vcvars.c_str()));
			bat.append("SET\r\n");
			file_put_contents(bf, bat);
		}
		out.clear();
		err.clear();
		if (ssu::exec(out, err, bf))
		{
			return	false;
		}
		for (auto line : split_chars(out, "\n"))
		{
			line	= trim(line, "\r\n\t ");
			
			std::string_view	key;
			std::string_view	value;
			auto	r = wcmatch(line, std::string_view("(+)=(+)"), [&](auto& m)
			{
				if (m.index == 1)
				{
					key		= m.match;
				}
				if (m.index == 2)
				{
					value	= m.match;
				}
			});
			setenv(key, value);
		}

		return	true;
	}
};




struct	license
{
	char const*	name;
	char const*	text;
};
#include <amtrs/amtrs-license.hpp>


static optinfo	sOptions[] = {
	{ 'h', "help",               opttype::none,		"-h is short help, --help is long help."                  },
	{ 'a', "libamtrs-dir",       opttype::none,		"libamtrs install directory."                             },
	{ 'e', "buildwork-clean",    opttype::none,		"Cleanup build work directory with build complited."      },
	{ 'm', "show-build-message", opttype::none,		"Show build command messages on time."                    },
	{ 'b', "build-always",       opttype::none,		"Build always. No check file modified."                   },
	{ 'c', "config",             opttype::required,	"Select libamtrs config."                                 },
	{ 'C', "show-config",        opttype::none,	    "Show config settings. Not running script."               },
	{ 'i', "node-install",       opttype::none,	    "No install developper tools and libamtrs and libraries." },
	{ 'u', "update",             opttype::none,	    "Update developper tools and libamtrs and libraries."     },
	{ 'l', "licenses",           opttype::none,     "Licenses print."                                         },
};

int initconfig(config& cnf, int _argc, char** _args)
{
	std::string	name		= _args[0];
	--_argc;
	++_args;

	cnf.libamtrsDir			= getenv<std::string>("LIBAMTRS_DIR");
	cnf.keepBuildWork		= true;

	optstate	o;
	while (optparse(o, _args, _argc, sOptions))
	{
		switch (o.shortval)
		{
			case (int)opterr::none_argumrnt :
				std::cerr << o.opt << " has not argument." << std::endl;
				return	1;

			case 'h' :
				if (o.longmode)
						std::cout << usage_long (sOptions, name, "filename...", "Instant executor for c++.") << std::endl;
				else	std::cout << usage_short(sOptions, name, "[filename...]") << std::endl;
				return	1;

			case 'e' :	cnf.keepBuildWork		= false;	break;
			case 'm' :	cnf.showBuildMessage	= true;		break;
			case 'c' :
			{
				if (!exists(o.arg))
				{
					std::cerr << "Can't open config file \"" << o.arg << "\"." << std::endl;
					return	1;
				}
				cnf.configPath	= o.arg;
				break;
			}

			case 'C' :	cnf.showConfig			= true;		break;
			case 'b' :	cnf.buildAlways			= true;		break;
			case 'i' :	cnf.noInstall			= true;		break;
			case 'u' :	cnf.updateMode			= true;		break;

			case 'l' :
			{
				for (auto l : licenses)
				{
					if (l)
					{
						std::cout << "# " << l->name << std::endl << std::endl;
						std::cout << l->text << std::endl;
					}
				}
				return	0;
			}

			default:
				return	1;
		}
	}

	if (_argc > 0)
	{
		cnf.scriptPath	= *_args++; --_argc;
		while (_argc > 0)
		{
			std::string	cmd(*_args++);	--_argc;
			cnf.srciptArgs.push_back(cmd);
		}
	}

	// コンフィグファイルの設定
	if (cnf.configPath.empty())
	{
		// default
		cnf.configPath	= absdir(special_path<std::string>(specialpathtype::home) + "/.amtrsconf");
		if (cnf.configPath.empty() || !exists(cnf.configPath))
		{
			cnf.configPath	= absdir(special_path<std::string>(specialpathtype::app_local, "libamtrs") + "/.amtrsconf");
			if (cnf.configPath.empty() || !exists(cnf.configPath))
			{
				cnf.configPath.clear();
			}
		}
	}


	if (!cnf.scriptPath.empty())
	{
		cnf.appname		= stem(cnf.scriptPath);
		cnf.scriptPath	= absdir(cnf.scriptPath);
	}

	// 特に設定が無ければデフォルトパスをセット
	if (cnf.cacheDir.empty())
	{
		cnf.cacheDir		= normalize_path(special_path<std::string>(specialpathtype::cache, std::string("libamtrs/.cache/") + cnf.appname));
	}
	if (cnf.buildWorkDir.empty())
	{
		cnf.buildWorkDir	= normalize_path(cnf.cacheDir + "/build/");
	}
	if (cnf.libamtrsDir.empty())
	{
		cnf.libamtrsDir	= getenv<std::string>("LIBAMTRS_DIR");
		if (!is_directory(cnf.libamtrsDir))
		{
			special_path<std::string>(cnf.libamtrsDir, specialpathtype::app_local, "libamtrs/libamtrs/");
		}
	}

	//
	visualstudio	vs;
	std::string		infodir	= cnf.cacheDir + "/.buildinfo";
	try { std::filesystem::create_directories(infodir); } catch (...) {}
	if (!vs.check_installed(infodir))
	{
	//	if (cnf.noInstall)
	//	{
	//		std::cerr << "Not installed c++ compiler or cmake." << std::endl;
	//		return	1;
	//	}
	//	setup_visualstudio(true, cnf);
	//	if (!vs.check_installed(infodir))
	//	{
	//		return	1;
	//	}
	}


	if (cnf.showConfig)
	{
		std::cerr << "appname    : " << cnf.appname << std::endl;
		std::cerr << "arguments  : [";
		for (auto& arg : cnf.srciptArgs)
		{
			std::cerr << "\"" << arg << "\", ";
		}
		std::cerr << "]" << std::endl;
		std::cerr << "build    : " << cnf.buildWorkDir << std::endl;
		std::cerr << "cache    : " << cnf.cacheDir << std::endl;
		std::cerr << "config   : " << cnf.configPath << std::endl;
		std::cerr << "libamtrs : " << cnf.libamtrsDir << std::endl;
		std::cerr << "script   : " << cnf.scriptPath << std::endl;

		std::cerr << "build    : " << vs.build << std::endl;
		std::cerr << "cmake    : " << vs.cmake << std::endl;

		return	1;
	}


	if (!is_setuped(cnf))
	{
		setup_visualstudio(true, cnf);

		if (!setup(cnf))
		{
			return	1;
		}
	}

	if (!is_directory(cnf.libamtrsDir))
	{
		std::cerr << "libamtrs directory \"" << cnf.libamtrsDir << "\" nothing." << std::endl;
		std::cerr << "libamtrs download and install from github >  " << _args[0] << " --install-libamtrs" << std::endl;
		return	1;
	}


#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	cnf.cmakeCmd		= ssu::add_exe("cmake")
						+ " " + cnf.cacheDir
						+ " -B \"" + cnf.buildWorkDir + "\""
						+ " -G \"NMake Makefiles\""
						+ " \"-DCMAKE_INSTALL_PREFIX=" + cnf.cacheDir + "\""
						+ " \"-DCMAKE_BUILD_TYPE=Release\""
	;
	cnf.buildChangeDir	= true;
	cnf.buildCmd		= std::string("nmake.exe install");
//	cnf.buildCmd		= std::string("msbuild.exe ") + cnf.buildWorkDir + "\\INSTALL.vcxproj  /p:Configuration=Release";
	cnf.buildTargetPath	= cnf.buildWorkDir + std::string("\\Release\\") + ssu::add_exe(cnf.appname);
#else
	cnf.cmakeCmd		= ssu::add_exe("cmake")
						+ " " + cnf.cacheDir
						+ " -B \"" + cnf.buildWorkDir + "\""
						+ " \"-DCMAKE_INSTALL_PREFIX=" + cnf.cacheDir + "\""
						+ " \"-DCMAKE_BUILD_TYPE=Release\""
	;
	cnf.buildCmd		= std::string("make -C ") + cnf.buildWorkDir + " install";
	cnf.buildTargetPath	= normalize_path(cnf.buildWorkDir + "/" + ssu::add_exe(cnf.appname));
#endif
	cnf.executePath		= normalize_path(cnf.cacheDir + "/" + ssu::add_exe(cnf.appname));
	return	0;
}
