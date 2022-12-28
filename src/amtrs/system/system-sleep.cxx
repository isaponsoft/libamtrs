/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <chrono>
#include <thread>
#include <amtrs/system/sleep.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN

void sleep_ms(int _ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
}

void sleep_ns(int _ns)
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(_ns));
}


AMTRS_SYSTEM_NAMESPACE_END
