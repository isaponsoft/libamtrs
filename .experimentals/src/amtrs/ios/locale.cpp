/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#import <UIKit/UIKit.h>
#include <amtrs/locale/locale.hpp>
#include <amtrs/.driver/ios/applekit/string.hpp>
AMTRS_NAMESPACE_BEGIN


struct	locale_impl : locale
{
	struct	instance : locale::instance
	{
		
	};
};

locale::locale()
{
}

locale::locale(locale&& _r)
	: mInstance(std::move(_r.mInstance))
{
}

locale& locale::operator = (locale&& _r)
{
	mInstance	= std::move(_r.mInstance);
	return	*this;
}

locale::~locale()
{
}

std::string locale::get_language() const
{@autoreleasepool {
	return	std::to_string([[NSLocale preferredLanguages] objectAtIndex: 0]);
}}


AMTRS_NAMESPACE_END
