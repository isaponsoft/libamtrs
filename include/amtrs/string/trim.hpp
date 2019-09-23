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
#ifndef	__libamtrs__string__trim__hpp
#define	__libamtrs__string__trim__hpp
#include <string_view>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(trim)
template<class CharT>
bool has(CharT _c, const CharT* _delms)
{
	while (*_delms)
	{
		if (*_delms++ == _c)
		{
			return	true;
		}
	}
	return	false;
}

AMTRS_IMPLEMENTS_END(trim)


template<class CharT, class TraitsT>
std::basic_string_view<CharT, TraitsT> trim(std::basic_string_view<CharT, TraitsT> _string, const CharT* _delms)
{
	if (_string.empty())
	{
		return	_string;
	}

	auto	begin	= _string.begin();
	auto	end		= _string.end();
	while (begin != end && AMTRS_IMPLEMENTS(trim)::has(*begin, _delms))
	{
		++begin;
	}
	if (begin != end)
	{
		while (begin != (end-1))
		{
			if (!AMTRS_IMPLEMENTS(trim)::has(*(end-1), _delms))
			{
				break;
			}
			--end;
		}
	}
	if (end == begin)
	{
		return	{};
	}
	return	{ &*begin, typename std::basic_string_view<CharT, TraitsT>::size_type(end - begin) };
}



AMTRS_NAMESPACE_END
#endif
