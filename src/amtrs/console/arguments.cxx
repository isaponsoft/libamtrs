/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/console/arguments.hpp>
AMTRS_CONSOLE_NAMESPACE_BEGIN

simplearray<sstring> parse_command_args(sstring const& e)
{
	simplearray<sstring>	ret;
	text					cur	= strutil::trim_blank(text(e));
	auto					beg	= cur.s;
	while (!cur.empty())
	{
		if (cur.front() == ' ')
		{
			ret.push_back({beg, cur.s});
			++cur;
			cur	= strutil::skip_blank(cur);
			beg	= cur.s;
		}
		else
		{
			++cur;
		}
	}
	if (beg != cur.s)
	{
		ret.push_back({beg, cur.s});
	}
	return	ret;
}


AMTRS_CONSOLE_NAMESPACE_END
