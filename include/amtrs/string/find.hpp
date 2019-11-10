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
#ifndef	__libamtrs__string__find__hpp
#define	__libamtrs__string__find__hpp
#include "starts_with.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! _a から _b が見つかる位置を返します。
// ----------------------------------------------------------------------------
template<class CharT, class Traits>
auto find_string_first_of(std::basic_string_view<CharT, Traits> _a, std::basic_string_view<CharT, Traits> _b) -> typename std::basic_string_view<CharT, Traits>::size_type
{
	// _a が _b より短ければ即終了
	if (_a.size() < _b.size())
	{
		return	std::basic_string_view<CharT, Traits>::npos;
	}

	auto	cur		= _a.data();
	auto	tail	= _a.data() + _a.size();
	auto	end		= tail - _b.size();
	for (;;)
	{
		if (starts_with(std::basic_string_view<CharT, Traits>(cur, tail - cur), _b))
		{
			return	cur - _a.data();
		}
		if (end == cur)
		{
			return	std::basic_string_view<CharT, Traits>::npos;
		}
		++cur;
	}
}



AMTRS_NAMESPACE_END
#endif
