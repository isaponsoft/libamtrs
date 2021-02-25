/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <amtrs/amtrs.hpp>
AMTRS_LOGGING_NAMESPACE_BEGIN


void write(loglevel _level, const char* _msg, std::size_t _size, bool _return)
{
	auto					len	= ::MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, _msg, (int)_size, nullptr, 0);
	std::vector<wchar_t>	buf	= std::vector<wchar_t>(static_cast<size_t>(len) + 1);

	::MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, _msg, (int)_size, buf.data(), len + 1);

	std::wcout << buf.data();
	if (_return)
	{
		std::wcout << std::endl;
	}
}


AMTRS_LOGGING_NAMESPACE_END
