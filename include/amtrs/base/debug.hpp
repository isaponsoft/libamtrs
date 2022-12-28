/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__base__debug__hpp
#define	__libamtrs__base__debug__hpp

// AMTRS_DEBUG				0 or 1
#ifndef	AMTRS_DEBUG
	#if	defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_) || defined(__DEBUG)
		#define	AMTRS_DEBUG		1
	#else
		#define	AMTRS_DEBUG		0
	#endif
#endif


// AMTRS_DEBUG_LOG_LEVEL	0 - 9
#if	!defined(AMTRS_DEBUG_LOG_LEVEL)
	#if		AMTRS_DEBUG
		#define	AMTRS_DEBUG_LOG_LEVEL	6
	#else
		#define	AMTRS_DEBUG_LOG_LEVEL	1
	#endif
#endif

// AMTRS_DEBUG_LOG_LEVEL_PRINT	0 or 1
#ifndef	AMTRS_DEBUG_LOG_LEVEL_PRINT
	#define	AMTRS_DEBUG_LOG_LEVEL_PRINT		1
	#define	AMTRS_DEBUG_LOG_LEVEL_FORMAT	"[{}] "
#endif

#endif
