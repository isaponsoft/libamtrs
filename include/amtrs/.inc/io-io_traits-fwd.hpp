/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__io_traits_fwd__hpp
#define	__libamtrs__io__io_traits_fwd__hpp
AMTRS_IO_NAMESPACE_BEGIN

template<class...>
struct	io_traits;			// prototype. 


template<class T>
using	io_traits_t	= io_traits<typename std::remove_reference<T>::type>;

AMTRS_IO_NAMESPACE_END
#endif
