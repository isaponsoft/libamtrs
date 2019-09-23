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
#ifndef	__libamtrs__string__format__hpp
#define	__libamtrs__string__format__hpp
#include <string>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(format)

template<class StringT, class... Args>
struct	format_impl
{
	StringT format(const typename StringT::value_type* _format, Args&&... _args)
	{
		using	string_type	= StringT;
		char	buff[1024];
		auto	len	= ::snprintf(buff, sizeof(buff), _format, std::forward<Args>(_args)...);
		return	string_type(std::string_view(buff, len));
	}
};


template<class StringT>
struct	format_impl<StringT>
{
	StringT format(const typename StringT::value_type* _format)
	{
		using	string_type	= StringT;
		return	string_type(_format);
	};
};
AMTRS_IMPLEMENTS_END(format)




template<class StringT, class... Args>
auto format(const typename StringT::value_type* _format, Args&&... _args) -> StringT
{
	using	imp	= AMTRS_IMPLEMENTS(format)::format_impl<StringT, Args...>;
	return	imp().format(_format, std::forward<Args>(_args)...);
};



AMTRS_NAMESPACE_END
#endif
