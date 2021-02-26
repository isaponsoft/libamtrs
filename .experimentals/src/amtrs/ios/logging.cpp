/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/logging/logging.hpp>
#import <os/log.h>
AMTRS_LOGGING_NAMESPACE_BEGIN


void write(loglevel _level, const char* _msg, std::size_t _size)
{
	os_log(OS_LOG_DEFAULT, "[lamtrs] %{public}s", _msg);
}


AMTRS_LOGGING_NAMESPACE_END
