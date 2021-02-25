/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <vector>
AMTRS_LOGGING_NAMESPACE_BEGIN

void output(loglevel _level, location _locate, std::string const& _msg, bool _return)
{
	auto				maxsize		= 8192;
	std::string_view	name		= _locate.filename;
	auto				pos			= name.size();
	for (int dircount = 2; dircount >= 1; --dircount)
	{
		auto	next	= name.find_last_of("/\\", pos - 1);
		if (next == std::string_view::npos)
		{
			break;
		}
		pos = next;
	}
	std::vector<char>	buff1;	buff1.resize(maxsize);
	std::vector<char>	buff2;	buff2.resize(maxsize);
	std::vector<char>	buff3;	buff3.resize(maxsize);
								  snprintf(buff1.data(), buff1.size(), "%s(%d)", name.data() + pos + 1, (int)_locate.line);
								  snprintf(buff2.data(), buff2.size(), "%s", _msg.c_str());
	auto				size3	= snprintf(buff3.data(), buff3.size(), "%s : %s", buff1.data(), buff2.data());
	write(_level, buff3.data(), size3);
}


void output(std::string const& _msg, bool _return)
{
	write(loglevel::none, _msg.data(), _msg.size(), _return);
}


AMTRS_LOGGING_NAMESPACE_END
