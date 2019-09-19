#ifndef	__libamtrs__net__simple_http__hpp
#define	__libamtrs__net__simple_http__hpp
#include <vector>
#include "types.hpp"
AMTRS_NET_NAMESPACE_BEGIN


class	simple_http
		: public ref_object
{
public:
	using	method		= http_method;
	using	value_type	= std::pair<std::string, std::string>;

	static ref<simple_http> create();

	//! 指定したURLへ GET のリクエストを行います。
	virtual void request_get(std::string_view _url) = 0;

	//! 最後の connect() のレスポンスコードを示します。
	int response_code() const noexcept { return mResponseCode; }

	//! 最後の connect() の Content-Length を示します。
	//! Content-Length を返さなかった場合は -1 を返します。
	std::intmax_t content_length() const noexcept { return mContentLength; }

	//! 最後の connect() の Content-Type を示します。
	//! Content-Type を返さなかった場合は empty() を返します。
	const std::string& content_type() const noexcept { return mContentType; }

	//! 最後の connect() の ボディを示します。
	//! Content-Type を返さなかった場合は empty() を返します。
	const std::string& content_body() const noexcept { return mContentBody; }



	//! 最後の connect() で得たヘッダを示します。
	const std::vector<value_type>& headers() const noexcept { return mHeaders; }

protected:
	int						mResponseCode	= -1;
	std::intmax_t			mContentLength	= -1;
	std::string				mContentType;
	std::string				mContentBody;
	std::vector<value_type>	mHeaders;
};

AMTRS_NET_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(simple_http)
#endif
