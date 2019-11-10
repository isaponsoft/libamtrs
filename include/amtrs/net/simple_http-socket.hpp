#ifndef	__libamtrs__net__simple_http_socket__hpp
#define	__libamtrs__net__simple_http_socket__hpp
#include "socket/@"
AMTRS_NET_NAMESPACE_BEGIN


class	simple_http_socket
		: public simple_http
{
public:


	//! 指定したURLへ GET のリクエストを行います。
	virtual void request_get(std::string_view _url) override
	{

	}

	friend	class simple_http;

protected:
	socket		mSock;
};



AMTRS_NET_NAMESPACE_END
#endif
