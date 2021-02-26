﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__hpp
#define	__libamtrs__net__hpp
#include "amtrs.hpp"


#include ".inc/typeutil-listener.hpp"

#define	AMTRS_NET_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace net {
#define	AMTRS_NET_NAMESPACE_END		} AMTRS_NAMESPACE_END
AMTRS_NET_NAMESPACE_BEGIN
AMTRS_NET_NAMESPACE_END

#include ".inc/net-types.hpp"
#include ".inc/net-uri.hpp"
#include ".inc/net-address.hpp"
#include ".inc/net-network_listener.hpp"

#include ".inc/net-socket.hpp"
#include ".inc/net-socket-connection.hpp"
#include ".inc/net-socket-stream.hpp"
#include ".inc/net-socket-ssl_stream.hpp"


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

#include ".inc/net-http-hpack.hpp"
#include ".inc/net-http-parser.hpp"
#include ".inc/net-http-http_header_analyzer.hpp"
#include ".inc/net-http-mini_http.hpp"
#include ".inc/net-simple_http.hpp"
#include ".inc/net-web.hpp"


#endif
