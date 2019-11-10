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
#ifndef	__libamtrs__string__sstring__hpp
#define	__libamtrs__string__sstring__hpp
#include <string_view>
#include "def.hpp"


// ****************************************************************************
//! std::string によく似たクラス
// ----------------------------------------------------------------------------
//! std::string によく似た機能を提供しますが、std::string はオブジェクトごとに
//! 独立したメモリを持つのに対し、sstring は共有メモリで管理されます。
//! そのためコピーが早いなどのメリットがあります。
// ----------------------------------------------------------------------------
AMTRS_NAMESPACE_BEGIN
template<class CharT>
struct	sstring_text : ref_object
{
	
};






template<class CharT, class Traits = std::char_traits<CharT>>
class	basic_sstring
		: protected shared_buffer<CharT>
{
public:
	using	super_type	= shared_buffer<CharT>;
	using	char_type	= CharT;
	using	view_type	= std::basic_string_view<CharT, Traits>;
	using	size_type	= size_t;


	basic_sstring() = default;
	basic_sstring(const basic_sstring&) = default;
	basic_sstring(basic_sstring&&) = default;
	basic_sstring& operator = (const basic_sstring&) = default;
	basic_sstring& operator = (basic_sstring&&) = default;

	basic_sstring(view_type _s)
		: super_type(_s.size())
	{
		std::copy_n(_s.data(), _s.size(), super_type::data());
	}

	basic_sstring(char_type const* _s, size_type _len)
		: basic_sstring(view_type(_s, _len))
	{}

	template<class S>
	basic_sstring(const S& _s)
		: basic_sstring(view_type(_s))
	{}

	template<class S>
	basic_sstring& operator = (const S& _s)
	{
		*this	= basic_sstring(_s);
		return	*this;
	}

	template<class S>
	bool operator == (const S& _s) const noexcept
	{
		return	(view_type)(*this) == (view_type)(_s);
	}

	template<class S>
	bool operator != (const S& _s) const noexcept
	{
		return	(view_type)(*this) != (view_type)(_s);
	}

	operator view_type () const noexcept
	{
		return	view_type(data(), size());
	}

	char_type const* data() const noexcept
	{
		return	super_type::data();
	}

	size_type size() const noexcept
	{
		return	super_type::size();
	}

protected:
};
using	sstring		= basic_sstring<char>;
using	swstring	= basic_sstring<wchar_t>;


AMTRS_NAMESPACE_END
#endif
