/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#import <UIKit/UIKit.h>
#include <amtrs/system/env.hpp>
AMTRS_SYSTEM_NAMESPACE_BEGIN

sstring get_locale_rfc3282()
{@autoreleasepool {
	//	NSString*	locale	= [[NSLocale currentLocale] objectForKey:NSLocaleIdentifier];
	NSString*	locale	= [[NSLocale preferredLanguages] objectAtIndex: 0];
	const char*	str		= [locale UTF8String];
	return	(string)str;
}};

AMTRS_SYSTEM_NAMESPACE_END
