#ifndef	__libamtrs__net__types__hpp
#define	__libamtrs__net__types__hpp
#include "def.hpp"
AMTRS_NET_NAMESPACE_BEGIN

enum class http_method
{
	HTTP_GET,
	HTTP_HEAD,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_CONNECT,
	HTTP_OPTIONS,
	HTTP_TRACE,
	HTTP_LINK,
	HTTP_UNLINK
};

enum class http_responce_code
{
	OK			= 200,
	NOT_FOUND	= 400,
};


AMTRS_NET_NAMESPACE_END
#endif
