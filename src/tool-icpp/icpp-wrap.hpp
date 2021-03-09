/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
static char const	gMainCpp[] =
R"(/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
// include list
#include <amtrs/scriptutil.hpp>
#include <list>
#include <iostream>
##INCLUDELIST##

namespace ssu = AMTRS_SCRIPTUTIL_NAMESPACE;

int script(ssu::global& __g__);

int main(int _argc, char** _args)
{
	ssu::global	g;
	for (int i = 0; i < _argc; ++i)
	{
		g.argv.push_back(std::string(_args[i]));
	}
	try
	{
		return	script(g);
	}
	catch (...)
	{
		std::cerr << _args[0] << " exception." << std::endl;
	}
	return	255;
}

int script(ssu::global& __g__)
{
	auto&	argv(__g__.argv);
#line ##SCRIPTLINE## "##_SCRIPTFILE_##"
##_SCRIPTDATA_##
	return	0;
}

)";


static char const	gCMakeListsTxt[] = 
R"(cmake_minimum_required(VERSION 3.0.2)
project(##PRJNAME##)

set(AMTRS_APPNAME "##PRJNAME##")

file(GLOB AMTRS_APPSRC
	main.cpp
)

set(AMTRS_SSL_ENABLE 1)
set(AMTRS_ZLIB_ENABLE 1)
set(AMTRS_ARCHIVE_ENABLE 1)
set(AMTRS_CONSOLE_ENABLE 1)
set(libAmtrs_DIR ##LIAMTRSDIR##)
include(${libAmtrs_DIR}/template.app-console.cmake)


target_include_directories(##PRJNAME## PUBLIC "##SRCDIR##")

install(TARGETS ##PRJNAME## DESTINATION .)
)";
