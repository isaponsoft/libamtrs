/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__compati__variant__variant__hpp
#define	__libamtrs__compati__variant__variant__hpp
#include "types.hpp"

// C++17 compatible
#if		defined(AMTRS_HAVE_STL17)
	#if		defined(_MSC_VER)
		// Microsoft VisualC++
		#define	AMTRS_HAVE_VARIANT
	#endif
#endif// defined(AMTRS_HAVE_STL17)

#if		defined(AMTRS_HAVE_VARIANT)
#	include <experimental/variant>
#endif

#endif
