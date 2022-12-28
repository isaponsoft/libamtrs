/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__chartype__hpp
#define	__libamtrs__string__chartype__hpp
AMTRS_NAMESPACE_BEGIN


template<class...>
struct	chartype;


template<>
struct	chartype<char>
{
	using	type	= char;
};

template<class T>
struct	chartype<T*>
{
	using	type	= T;
};


template<class T>
struct	chartype<T>
{
	using	type	= typename T::value_type;
};




template<class... T>
using	chartype_t	= typename chartype<typename std::remove_reference<T>::type...>::type;


AMTRS_NAMESPACE_END
#endif
