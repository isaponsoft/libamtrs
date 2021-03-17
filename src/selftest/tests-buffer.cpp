/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/filesystem.hpp>


int test_buffer()
{
	std::string	s;
	if (!amtrs::filesystem::file_get_contents(s, "test.txt"))
	{
		std::cout << "error" << std::endl;
		return	1;
	}

	std::cout << s << std::endl;

	amtrs::filesystem::file_put_contents("test2.txt", s);

	std::string	s2;
	amtrs::filesystem::file_get_contents(s2, "test2.txt");

	std::cout << s2 << std::endl;

	return	0;
}

