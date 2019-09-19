/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__web__web_ios__hpp
#define	__libamtrs__web__web_ios__hpp
#import <UIKit/UIKit.h>
#include "../../amtrs.hpp"
AMTRS_NAMESPACE_BEGIN

struct	web
{
	static void jump_to(const std::string& _url)
	{
		@autoreleasepool
		{
			NSString*	string	= [NSString stringWithCString: _url.c_str() encoding:NSUTF8StringEncoding];
			NSURL*		url		= [NSURL URLWithString:string];
/*
			[[UIApplication sharedApplication]
					openURL:			url
			];
*/
			[[UIApplication sharedApplication]
					openURL:			url
					options:			@{}
					completionHandler:	nil
			];
		}
	}
};



AMTRS_NAMESPACE_END
#endif
