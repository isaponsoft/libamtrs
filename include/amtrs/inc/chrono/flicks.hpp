/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__chrono__fricks__hpp
#define	__libamtrs__chrono__fricks__hpp
#include <chrono>
#include "def.hpp"
AMTRS_CHRONO_NAMESPACE_BEGIN

// https://github.com/OculusVR/Flicks
using		flicks	= std::chrono::duration<std::chrono::nanoseconds::rep, std::ratio<1, 705600000>>;

AMTRS_CHRONO_NAMESPACE_END
#endif
