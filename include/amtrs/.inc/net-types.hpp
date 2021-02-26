/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__net__types__hpp
#define	__libamtrs__net__types__hpp
AMTRS_NET_NAMESPACE_BEGIN

enum class	protocol
{
	http,
	https,
};

enum class	http_method
{
	GET,
	POST,
	PUT,
	DEL,
	HEAD,
	OPTIONS,
	TRACE,
	CONNECT,
};


enum class	url_encode_type
{
//	RFC2396					= 1,
	RFC3986					= 2,
};


enum class	http_statuscode
{
	// 1xx
	Continue					= 100,
	SwitchingProtocols			= 101,

	// 2xx
	OK							= 200,
	Created						= 201,
	Accepted					= 202,
	NonAuthoritativeInformation	= 203,
	NoContent					= 204,
	ResetContent				= 205,
	PartialContent				= 206,

	// 3xx
	MultipleChoices				= 300,
	MovedPermanently			= 301,
	MovedTemporarily			= 302,
	SeeOther					= 303,
	NotModified					= 304,
	UseProxy					= 305,

	// 4xx
	BadRequest					= 400,
	Unauthorized				= 401,
	PaymentRequired				= 402,
	Forbidden					= 403,
	NotFound					= 404,
	MethodNotAllowed			= 405,
	NotAcceptable				= 406,
	ProxyAuthenticationRequired	= 407,
	RequestTimeOut				= 408,
	Conflict					= 409,
	Gone						= 410,
	LengthRequired				= 411,
	PreconditionFailed			= 412,
	RequestEntityTooLarge		= 413,
	RequestURITooLarge			= 414,
	UnsupportedMediaType		= 415,

	// 5xx
	InternalServerError			= 500,
	NotImplemented				= 501,
	BadGateway					= 502,
	SeviceUnavailable			= 503,
	GatewayTimeOut				= 504,
	HTTPVersionnotsupported		= 505,
};

AMTRS_NET_NAMESPACE_END
#endif
