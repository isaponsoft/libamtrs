/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <sstream>
#include <amtrs/tests.hpp>
#include <amtrs/lang/psnvm.hpp>
#include <amtrs/string/join.hpp>
using namespace amtrs;
using namespace psnvm;
using namespace psnvm::lang;
using	ec	= errorcode;
#define	_PRFX(_name)	lang_passion_prepro_##_name
namespace { sstring go(text _src); }

#define	OKTEST(_src, _answer)	\
		{\
			auto	r	= go(_src);\
			auto	t	= ((text)r).trim("\r\n\t ");\
			auto	a	= ((text)#_answer).trim("\r\n\t ");\
			if (logprint) { std::cout << std::endl << "(Line " << __LINE__ << ")" << std::endl << a << std::endl; }\
			AMTRS_TEST_EQ(t, a);\
		}


AMTRS_TEST_F(_PRFX(0))
{
	bool logprint	= true;
	// macro
	{
		OKTEST(R"(
			#define	ABC	abc
			ABC
		)",
			abc
		)
	}
	{
		OKTEST(R"(
			#define	ABC	abc
			#define	CDE	ABC
			CDE
		)",
			abc
		)
	}
	{
		OKTEST(R"(
			#define	ABC	abc
			#undef	ABC
			ABC
		)",
			ABC
		)
	}
}


namespace {

sstring go(text _src)
{
	prepro	pp;
	pp.line_directive	= false;
	pp.removed_line		= "";
	return	pp.process(_src, "testcode");
}

}
