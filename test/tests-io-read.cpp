/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <cstring>
#include <string.h>
#include <amtrs/io.hpp>
#include <amtrs/tests.hpp>
#include <fstream>
using namespace amtrs;

#define	_PRFX(_name)	io_read_##_name



AMTRS_TEST_F(_PRFX(endian))
{
	char	buf[8]	= {0,1,2,3,4,5,6,7};
	auto	in		= io::make_streamif(std::string_view(buf, sizeof(buf)));


	uint16_t	n16;
	AMTRS_TEST_TRUE(io::read<endian::big>(n16, in));
	AMTRS_TEST_EQ(0x0001, n16);

	AMTRS_TEST_TRUE(io::read<endian::little>(n16, in));
	AMTRS_TEST_EQ(0x0302, n16);

	uint32_t	n32;
	AMTRS_TEST_TRUE(io::read<endian::little>(n32, in));
	AMTRS_TEST_EQ(0x07060504, n32);
}

