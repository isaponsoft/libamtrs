/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__datetime__datetime_ios__hpp
#define	__libamtrs__datetime__datetime_ios__hpp
#include <time.h>
#import <UIKit/UIKit.h>
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! NSDate から unixtime を取得します。
// ----------------------------------------------------------------------------
inline std::time_t to_time(NSDate* _date)
{
	return	[_date timeIntervalSince1970];
}


// ============================================================================
//! NSDate から std::time_t を取得します。
// ----------------------------------------------------------------------------
inline struct tm localtime(NSDate* _date)
{
	struct tm	retval;
	auto	t	= to_time(_date);
	::localtime_r(&t, &retval);
	return	retval;
}


// ============================================================================
//! datetimeから NSDate へ変換します。
// ----------------------------------------------------------------------------
inline NSDate* to_NSDate(const datetime& _dt)
{
	return	[NSDate dateWithTimeIntervalSince1970:_dt.unixtime()];
}



AMTRS_NAMESPACE_END
#endif
