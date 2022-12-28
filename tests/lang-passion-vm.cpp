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
#define	_PRFX(_name)	lang_passion_vm_##_name


AMTRS_TEST_F(_PRFX(0))
{
	vmmemorymap	m;

	{
		void*		m1	= new char[100];
		void*		m2	= new char[100];
		m.add({m1, (char*)m1 + 100});
		m.add({m2, (char*)m2 + 100});
		auto	r1	= m.release(m1);
		auto	r2	= m.release(m2);
		auto	r3	= m.release(m2);
		AMTRS_TEST_EQ(r1.begin, m1);
		AMTRS_TEST_EQ(r2.begin, m2);
		AMTRS_TEST_EQ(r3.begin, nullptr);
	}

	{
		void*		m1	= new char[100];
		void*		m2	= new char[100];
		m.add({m1, (char*)m1 + 100});
		m.add({m2, (char*)m2 + 100});
		auto	r1	= m.release((char*)m1 + 99);
		auto	r2	= m.release((char*)m2 + 10);
		AMTRS_TEST_EQ(r1.begin, m1);
		AMTRS_TEST_EQ(r2.begin, m2);
	}

}
