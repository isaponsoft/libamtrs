/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__io_traits__hpp
#define	__libamtrs__io__io_traits__hpp

#include "io-io_traits-fwd.hpp"
#include "io-io_traits-stl.hpp"
#include "io-io_traits-view.hpp"


#if	__has_include(<unistd.h>)
#include "io-io_traits-unix.hpp"
#else
#include "io-io_traits-ms.hpp"
#endif
#endif
