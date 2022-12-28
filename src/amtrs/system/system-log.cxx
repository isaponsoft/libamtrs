/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <vector>
#include <amtrs/filesystem/path.hpp>
#include <amtrs/system/log.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN

void write_log(loglevel _level, const char* _msg, size_t _size, bool _return = true);

void output(loglevel _level, location _locate, text _msg, bool _return)
{
	auto	maxsize		= 8192;
	sstring	name		= sstring(filesystem::filename(_locate.filename));

	std::vector<char>	buff1;	buff1.resize(maxsize);
	std::vector<char>	buff2;	buff2.resize(maxsize);
	std::vector<char>	buff3;	buff3.resize(maxsize);
								  snprintf(buff1.data(), buff1.size(), "%s(%d)", name.c_str(), (int)_locate.line);
								  snprintf(buff2.data(), buff2.size(), "%s", _msg.data());
	auto				size3	= snprintf(buff3.data(), buff3.size(), "%s : %s", buff1.data(), buff2.data());
	write_log(_level, buff3.data(), size3);
}


void output(text _msg, bool _return)
{
	write_log(loglevel::none, _msg.data(), _msg.size(), _return);
}


auto loglevel_str(loglevel _level) -> char const*
{
	switch (_level)
	{
		case loglevel::none :			return	"ALL";
		case loglevel::critical :		return	"CRI";
		case loglevel::error :			return	"ERR";
		case loglevel::warn :			return	"WRN";
		case loglevel::info :			return	"INF";
		case loglevel::debug :			return	"DEB";
		case loglevel::trace :			return	"TRC";
		case loglevel::systemtrace :	return	"SYS";
		default:						return	"   ";
	}
}


AMTRS_SYSTEM_NAMESPACE_END
