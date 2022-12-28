/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__unixdrv__net_simple_http__hpp
#define	__libamtrs__driver__unixdrv__net_simple_http__hpp
#include "socket-net-simple_http.hpp"
AMTRS_NET_NAMESPACE_BEGIN


inline ref<simple_http> simple_http::create()
{
	return	new simple_http_socket();
}


AMTRS_NET_NAMESPACE_END
#endif
