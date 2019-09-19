/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__variant__variant__hpp
#define	__libamtrs__variant__variant__hpp
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
