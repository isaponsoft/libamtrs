/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__logging__logging_ios__hpp
#define	__libamtrs__logging__logging_ios__hpp
#include <iostream>
#import <os/log.h>
AMTRS_LOGGING_NAMESPACE_BEGIN

class	logging_ios
{
public:
	static void log(const char* _msg, std::size_t _size)
	{
/*
		static	bool		_init;
		static	os_log_t	_log;
		if (!_init)
		{
			_log	= os_log_create("jp.libamtrs", "log");
			_init	= true;
		}
		os_log(_log, "[lamtrs] %s", _msg);
*/
		os_log(OS_LOG_DEFAULT, "[lamtrs] %{public}s", _msg);
	}
};


inline auto config::max_size() noexcept -> size_type
{
	return	nosize;
}


inline void config::output(const char* _msg, size_type _size)
{
	logging_ios::log(_msg, _size);
}



AMTRS_LOGGING_NAMESPACE_END
#endif
