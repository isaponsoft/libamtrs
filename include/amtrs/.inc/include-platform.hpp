/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#if			__has_include(AMTRS_DRIVER_CUSTOM_INCLUDE(AMTRS_CURRENT_PLATFORM_NAME-AMTRS_PLIB_NAME))
#include	AMTRS_DRIVER_CUSTOM_INCLUDE(AMTRS_CURRENT_PLATFORM_NAME-AMTRS_PLIB_NAME)
#elif		defined(AMTRS_CURRENT_PLATFORM2_NAME)
#if			__has_include(AMTRS_DRIVER_CUSTOM_INCLUDE(AMTRS_CURRENT_PLATFORM2_NAME-AMTRS_PLIB_NAME))
#include	AMTRS_DRIVER_CUSTOM_INCLUDE(AMTRS_CURRENT_PLATFORM2_NAME-AMTRS_PLIB_NAME)
#endif
#endif
#undef	AMTRS_PLIB_NAME
