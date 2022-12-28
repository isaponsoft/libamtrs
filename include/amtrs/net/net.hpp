/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__hpp
#define	__libamtrs__net__hpp
#include "amtrs.hpp"
#include <unordered_map>
#include <vector>

#include "io.hpp"
#include "string.hpp"
#include ".inc/typeutil-listener.hpp"

#define	AMTRS_NET_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace net {
#define	AMTRS_NET_NAMESPACE_END		} AMTRS_NAMESPACE_END
AMTRS_NET_NAMESPACE_BEGIN
AMTRS_NET_NAMESPACE_END

#include ".inc-net/net-types.hpp"
#include ".inc-net/net-uri.hpp"
#include ".inc-net/net-address.hpp"
#include ".inc-net/net-network_listener.hpp"

#include ".inc-net/net-socket.hpp"
#include ".inc-net/net-socket-connection.hpp"
#include ".inc-net/net-socket-stream.hpp"
#include ".inc-net/net-socket-ssl_stream.hpp"


#define	AMTRS_NET_HTTP_NAMESPACE_BEGIN	AMTRS_NET_NAMESPACE_BEGIN namespace http {
#define	AMTRS_NET_HTTP_NAMESPACE_END	} AMTRS_NET_NAMESPACE_END
AMTRS_NET_HTTP_NAMESPACE_BEGIN

enum class transfer_encoding_type
{
	chunked,
	compress,		//!< LZW
	deflate,		//!< zlib/deflate
	gzip,			//!< LZ77
	identity,		//!< no compress
};


AMTRS_NET_HTTP_NAMESPACE_END

#include ".inc-net/net-http-hpack.hpp"
#include ".inc-net/net-http-parser.hpp"
#include ".inc-net/net-http-http_header_analyzer.hpp"
#include ".inc-net/net-http-mini_http.hpp"
#include ".inc-net/net-simple_http.hpp"


#endif
