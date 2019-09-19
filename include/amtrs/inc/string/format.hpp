/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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
