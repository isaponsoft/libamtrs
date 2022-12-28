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

static bool createdir(std::string _dir)
{
	try
	{
		if (!_dir.empty() && !std::filesystem::is_directory(_dir))
		{
			std::filesystem::create_directories(_dir);
		}
		return	true;
	}
	catch (...)
	{
		std::cout << "Can't create directory \"" << _dir << "\"." << std::endl;
		return	false;
	}
}

static bool download_and_unzip(bool _exec, std::string const& _url, std::string const& _destdir)
{
	bool	rstate	= true;
	std::string	destdir	= normalize_path(_destdir);
	if (!is_directory(destdir))
	{
		std::string	tmpname	= destdir + ".download";
		try
		{
			rstate	= false;
			if (!_exec)
			{
				return	false;
			}

			if (!createdir(std::string(filesystem::parent_path(destdir))))
			{
				throw	1;
			}
			if (!filesystem::exists(tmpname))
			{
				if (!ssu::download(_url, tmpname)) { throw 1; }
			}
			if (!createdir(destdir))
			{
				throw	1;
			}

			std::ifstream	in(tmpname, std::ios::binary);
			if (!in.good())
			{
				std::cout << "Can't open " << tmpname << std::endl;
				throw	1;
			}

			io::vstreamif	vin(io::make_vstreamif(io::make_streamif(std::move(in))));
			if (!ssu::extract(vin, destdir, { .sourcename = std::string(filesystem::filename(tmpname)), .skiproot = true }))
			{
				std::cout << "Extract error " << tmpname << " to " << destdir << std::endl;
				throw	1;
			}

			rstate	= true;
		}
		catch (...)
		{
			rstate	= false;
			try { std::filesystem::remove_all(tmpname); } catch (...) {}
			try { std::filesystem::remove_all(destdir); } catch (...) {}
		}
	}
	return	rstate;
}

#define	_PACKAGE(_dir, _url)	\
	if (!download_and_unzip(_exec, _url, cnf.libamtrsDir + "/" _dir))\
	{\
		return	false;\
	}\

static bool _setup(bool _exec, config& cnf)
{
	_PACKAGE(""               , "https://github.com/isaponsoft/libamtrs/archive/master.zip")
	_PACKAGE("deps/zlib"      , "https://github.com/madler/zlib/archive/master.zip")
	_PACKAGE("deps/libarchive", "https://github.com/libarchive/libarchive/archive/master.zip")
	_PACKAGE("deps/libressl"  , "https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.3.1.tar.gz")
	return	true;
}

bool is_setuped(config& cnf)
{
	return	_setup(false, cnf);
}

bool setup(config& cnf)
{
	return	_setup(true, cnf);
}
