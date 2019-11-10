/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__io__io_traits__hpp
#define	__libamtrs__io__io_traits__hpp

#include "io_traits-fwd.hpp"
#include "io_traits-stl.hpp"
#include "io_traits-view.hpp"


#if	__has_include(<unistd.h>)
#include "io_traits-unix.hpp"
#else
#include "io_traits-ms.hpp"
#endif
#endif
