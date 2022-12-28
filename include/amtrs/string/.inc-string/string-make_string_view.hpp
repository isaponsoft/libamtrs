/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__make_string_view__hpp
#define	__libamtrs__string__make_string_view__hpp
AMTRS_NAMESPACE_BEGIN


template<class T, class TraitsT = std::char_traits<T>>
auto make_string_view(T* _value, std::size_t _size) -> std::basic_string_view<T, TraitsT>
{
	return	{_value, _size};
}


AMTRS_NAMESPACE_END
#endif
