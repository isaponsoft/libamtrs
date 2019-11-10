/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__io_traits_fwd__hpp
#define	__libamtrs__io__io_traits_fwd__hpp
#include <iosfwd>
AMTRS_IO_NAMESPACE_BEGIN

template<class...>
struct	io_traits;			// prototype. 


template<class T>
using	io_traits_t	= io_traits<typename std::remove_reference<T>::type>;

AMTRS_IO_NAMESPACE_END
#endif
