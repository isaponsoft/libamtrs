/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__char8_t__hpp
#define	__libamtrs__string__char8_t__hpp

#if	__cpp_char8_t
#else
/*
 * fake u8string.
 */
using	char8_t		= amtrs::strong_typedef<char>;
namespace std
{
	using	u8string		= basic_string<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>;
}
#endif
AMTRS_NAMESPACE_BEGIN
inline std::string   const& cast_to_string  (std::string   const& s) { return s; }
inline std::string          cast_to_string  (std::u8string const& s) { return std::string(s.begin(), s.end()); }
inline std::u8string const& cast_to_u8string(std::u8string const& s) { return s; }
inline std::u8string        cast_to_u8string(std::string   const& s) { return std::u8string(s.begin(), s.end()); }
AMTRS_NAMESPACE_END
#endif
