/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <string>
#include <sstream>

#include <amtrs/system/log.hpp>
#include <amtrs/tests.hpp>
#include <amtrs/io/reader.hpp>
#include <amtrs/io/writer.hpp>
using namespace amtrs;

#define	_PRFX(_name)	io_reader_writer##_name


AMTRS_TEST_F(_PRFX(1_writer))
{
	std::stringstream	ss;
	char				txt[]	= "Hello world.";
	auto				w	= io::make_writer((std::ostream*)&ss);
	w->write(txt, sizeof(txt)-1);		// without \0
	AMTRS_TEST_EQ(ss.str(), txt)
}

AMTRS_TEST_F(_PRFX(1_reader))
{
	std::stringstream	ss;
	ss << "Hello world.";
	auto				r	= io::make_reader((std::istream*)&ss);
	char				buff[100] = {0};
	auto	s	= r->read(buff, sizeof(buff));
	AMTRS_TEST_EQ(text(buff, s), "Hello world.")
}
