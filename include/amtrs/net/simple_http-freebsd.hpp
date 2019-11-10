#ifndef	__libamtrs__net__simple_http_linux__hpp
#define	__libamtrs__net__simple_http_linux__hpp
#include "simple_http-socket.hpp"
AMTRS_NET_NAMESPACE_BEGIN


inline ref<simple_http> simple_http::create()
{
	return	new simple_http_socket();
}


AMTRS_NET_NAMESPACE_END
#endif
