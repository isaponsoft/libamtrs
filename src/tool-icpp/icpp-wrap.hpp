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

namespace ssu = AMTRS_SCRIPTUTIL_NAMESPACE;


#include "script.hpp"


template<class...>
struct	_imain
		: std::false_type
{
	static int invoke(...);
};

template<>
struct	_imain<int()>
		: std::true_type
{
	template<class F>
	static int invoke(F f, int, char**)
	{
		return	f();
	}
};

template<>
struct	_imain<int(int, char**)>
		: std::true_type
{
	template<class F>
	static int invoke(F f, int c, char** a)
	{
		return	f(c, a);
	}
};


int main(int c, char** a)
{
	int	e	= 255;
	try
	{
		static constexpr bool	imain_found	= _imain<decltype(imain)>::value;
#line 0 "icpp"
static_assert(imain_found, "NOT FOUND 'imain()', 'imain(int,char**)'.");
			e	= _imain<decltype(imain)>::invoke(&imain, c, a);
	}
	catch (...)
	{
	}
	return	e;
}
)";


static char const	gCMakeListsTxt[] = 
R"(cmake_minimum_required(VERSION 3.10)
project(##PRJNAME##)

set(AMTRS_APPNAME "##PRJNAME##")

file(GLOB AMTRS_APPSRC
	main.cpp
	script.hpp
)

set(libAmtrs_DIR ##LIAMTRSDIR##)
include(${libAmtrs_DIR}/libamtrs/template.app-console.cmake)


target_include_directories(##PRJNAME## PUBLIC "##SRCDIR##")

install(TARGETS ##PRJNAME## DESTINATION .)
)";
