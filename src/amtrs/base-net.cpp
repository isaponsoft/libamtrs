/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/system.hpp>
#include <amtrs/io.hpp>
#include <amtrs/net.hpp>
static std::atomic<int>	sUsingCount;

AMTRS_NET_NAMESPACE_BEGIN

netbase::netbase()
{
	if (++sUsingCount == 1)
	{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
		WSADATA	ws = {};
		int	err	= WSAStartup(MAKEWORD(2,0), &ws);
		if (err)
		{
			throw	std::error_code(err, std::generic_category());
		}
#endif
	}
}


netbase::~netbase()
{
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_WIN32
	if (--sUsingCount == 0)
	{
		WSACleanup();
	}
#endif
}

AMTRS_NET_NAMESPACE_END
