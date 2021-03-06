/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>
#include <amtrs/io.hpp>
#include <amtrs/tests.hpp>
using namespace amtrs;

#define	_PRFX(_name)	filesystem_zip##_name

static uint8_t	zipdata[] = {
#include "tests-filesystem-zip-test.zip.hpp"
};


AMTRS_TEST_F(_PRFX(filescan))
{
	std::string_view							zv((char*)zipdata, sizeof(zipdata));
	filesystem::zip::zip_archive<io::vstreamif>	z(io::make_vstreamif(io::make_streamif(zv)));
	AMTRS_TEST_TRUE(z);

	char const*	filelist[] =
	{
		"test/",
		"test/file1.txt",
		"test/file2.txt",
	};

	int	i	= 0;
	for (auto& e : z)
	{
		AMTRS_TEST_EQ(std::string_view(e.filename().data(), e.filename().size()), filelist[i]);
		++i;
	}
}
