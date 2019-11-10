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
#ifndef	__libamtrs__net__http__hpack__hpp
#define	__libamtrs__net__http__hpack__hpp
#include "def.hpp"
AMTRS_NET_HTTP_NAMESPACE_BEGIN


enum class	hpack_index
{
	authority						= 1,
	method_GET						= 2,
	method_POST						= 3,
	path_root						= 4,
	path_index_html					= 5,
	scheme_http						= 6,
	scheme_https					= 7,
	status_200						= 8,
	status_204						= 9,
	status_206						= 10,
	status_304						= 11,
	status_400						= 12,
	status_404						= 13,
	status_500						= 14,
	accept_charset					= 15,
	accept_encoding_zip_deflate		= 16,
	accept_language					= 17,
	accept_ranges					= 18,
	accept							= 19,
	access_control_allow_origin		= 20,
	age								= 21,
	allow							= 22,
	authorization					= 23,
	cache_control					= 24,
	content_disposition				= 25,
	content_encoding				= 26,
	content_language				= 27,
	content_length					= 28,
	content_location				= 29,
	content_range					= 30,
	content_type					= 31,
	cookie							= 32,	
	date							= 33,
	etag							= 34,
	expect							= 35,
	expires							= 36,
	from							= 37,
	host							= 38,
	if_match						= 39,
	if_modified_since				= 40,
	if_none_match					= 41,
	if_range						= 42,
	if_unmodified_since				= 43,
	last_modified					= 44,
	link							= 45,
	location						= 46,
	max_forwards					= 47,
	proxy_authenticate				= 48,
	proxy_authorization				= 49,
	range							= 50,
	referer							= 51,
	refresh							= 52,
	retry_after						= 53,
	server							= 54,
	set_cookie						= 55,
	strict_transport_security		= 56,
	transfer_encoding				= 57,
	user_agent						= 58,
	vary							= 59,
	via								= 60,
	www_authenticate				= 61,
};

AMTRS_NET_HTTP_NAMESPACE_END
#endif
