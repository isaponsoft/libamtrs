/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__logging__logging_android__hpp
#define	__libamtrs__logging__logging_android__hpp
#include <android/log.h>
AMTRS_LOGGING_NAMESPACE_BEGIN

inline auto config::max_size() noexcept -> size_type
{
	return	nosize;
}


inline void config::output(const char* _msg, size_type _size)
{
	__android_log_write(ANDROID_LOG_DEBUG, "lamtrs", _msg);
}



AMTRS_LOGGING_NAMESPACE_END
#endif
