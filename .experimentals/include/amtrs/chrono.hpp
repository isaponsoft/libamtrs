/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__chrono__hpp
#define	__libamtrs__chrono__hpp
#include "amtrs.hpp"

#include <ctime>
#include <chrono>
#include <functional>
#include <iomanip>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <thread>
#include "io/deserialize.hpp"
#include "io/serialize.hpp"

#define	AMTRS_CHRONO_NAMESPACE			AMTRS_NAMESPACE::chrono
#define	AMTRS_CHRONO_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace chrono {
#define	AMTRS_CHRONO_NAMESPACE_END		} AMTRS_NAMESPACE_END


#include AMTRS_PLATFORM_INCLUDE(chrono-time.hpp)

// keep order.
#include ".inc/chrono-functions.hpp"
#include ".inc/chrono-time.hpp"
#include ".inc/chrono-bedatetime.hpp"
#include ".inc/chrono-deltatimer.hpp"
#include ".inc/chrono-datetime.hpp"
#include ".inc/chrono-flicks.hpp"
#include ".inc/chrono-framerator.hpp"
#include ".inc/chrono-calendar.hpp"


#include AMTRS_PLATFORM_INCLUDE(chrono-datetime.hpp)

#endif
