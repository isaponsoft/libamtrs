/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__hpp
#define	__libamtrs__chrono__hpp
#include "../amtrs.hpp"

#include <ctime>
#include <chrono>
#include <functional>
#include <iomanip>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <thread>


#include ".inc-chrono/chrono-functions.hpp"
#include ".inc-chrono/chrono-time.hpp"

#if	0
#include AMTRS_PLATFORM_INCLUDE(chrono-time.hpp)


// keep order.
#include ".inc-chrono/chrono-functions.hpp"
#include ".inc-chrono/chrono-time.hpp"
#include ".inc-chrono/chrono-bedatetime.hpp"
#include ".inc-chrono/chrono-deltatimer.hpp"
#include ".inc-chrono/chrono-datetime.hpp"
#include ".inc-chrono/chrono-flicks.hpp"
#include ".inc-chrono/chrono-framerator.hpp"
#include ".inc-chrono/chrono-calendar.hpp"

#include AMTRS_PLATFORM_INCLUDE(chrono-datetime.hpp)
#endif
#endif
