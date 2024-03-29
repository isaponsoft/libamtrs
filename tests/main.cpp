﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/tests.hpp>
#include <fstream>
#include <clocale>

using namespace AMTRS_TEST_NAMESPACE;

int main(int _argc, char** _args)
{
	std::setlocale(LC_ALL, "");
	return	AMTRS_TEST_NAMESPACE::tests(_argc, _args);
}
