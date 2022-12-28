/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__to_string__hpp
#define	__libamtrs__string__to_string__hpp
AMTRS_NAMESPACE_BEGIN

template<class... T>
struct	cvtstring;


template<class T>
struct	cvtstring<T>
{
	template<class ValueT>
	inline std::string to(ValueT const& _value) const
	{
		return	std::to_string(_value);
	}
};


template<class T>
inline std::string to_string(T const& _value)
{
	return	cvtstring<T>{}.to(_value);
}

AMTRS_NAMESPACE_END
#endif
