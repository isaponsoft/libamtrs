/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <iostream>
#include <amtrs/string/convert.hpp>
#include <amtrs/system/log.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN


void write_log(loglevel _level, const char* _msg, std::size_t _size, bool _return)
{
	auto	ws	= c8_to_wc({_msg, _size});
	std::wcout << ws.data() << std::flush;
	if (_return)
	{
		std::wcout << std::endl;
	}
}


AMTRS_SYSTEM_NAMESPACE_END
