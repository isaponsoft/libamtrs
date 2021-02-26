/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <android/log.h>
AMTRS_LOGGING_NAMESPACE_BEGIN


void write(loglevel _level, const char* _msg, std::size_t _size, bool _return)
{
	android_LogPriority	pri	= _level == loglevel::critical	? ANDROID_LOG_FATAL
							: _level == loglevel::error		? ANDROID_LOG_ERROR
							: _level == loglevel::warn		? ANDROID_LOG_WARN
							: _level == loglevel::info		? ANDROID_LOG_INFO
							: _level == loglevel::debug		? ANDROID_LOG_DEBUG
							: ANDROID_LOG_VERBOSE;

	__android_log_write(ANDROID_LOG_DEBUG, "lamtrs", _msg);
}


AMTRS_LOGGING_NAMESPACE_END
