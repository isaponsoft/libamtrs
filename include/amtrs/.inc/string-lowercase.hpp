/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__lowercase__hpp
#define	__libamtrs__string__lowercase__hpp
AMTRS_NAMESPACE_BEGIN



template<class T, class S>
auto lowercase(S const& _s) -> T
{
	using	char_type	= chartype_t<S>;
	std::basic_string_view<char_type>	in(_s);
	T									retval;
	retval.resize(in.size());
	auto	d	= retval.data();
	auto	s	= in.data();
	for (size_t i = 0; i < in.size(); ++i)
	{
		d[i]	= (char_type)std::tolower(s[i]);
	}
	return	retval;
}


AMTRS_NAMESPACE_END
#endif
