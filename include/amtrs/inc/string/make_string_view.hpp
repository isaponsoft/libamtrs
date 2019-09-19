/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__string__make_string_view__hpp
#define	__libamtrs__string__make_string_view__hpp
#include <string_view>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<class T, class TraitsT = std::char_traits<T>>
auto make_string_view(T* _value, std::size_t _size) -> std::basic_string_view<T, TraitsT>
{
	return	{_value, _size};
}



AMTRS_NAMESPACE_END
#endif
