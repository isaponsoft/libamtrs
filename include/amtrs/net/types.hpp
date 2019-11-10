/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__net__types__hpp
#define	__libamtrs__net__types__hpp
#include "def.hpp"
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
