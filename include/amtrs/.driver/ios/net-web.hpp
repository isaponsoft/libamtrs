/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__web__web_ios__hpp
#define	__libamtrs__web__web_ios__hpp
#import <UIKit/UIKit.h>
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
