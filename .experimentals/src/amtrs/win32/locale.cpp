/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/amtrs.hpp>
#include <amtrs/locale/locale.hpp>
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
{
	CHAR	iso639 [10];
	CHAR	iso3166[10];
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME , iso639,  sizeof(iso639));
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, iso3166, sizeof(iso3166));
	char	rfc3282[8];
	snprintf(rfc3282, sizeof(rfc3282), "%s_%s", iso639, iso3166);
	return	rfc3282;
}


AMTRS_NAMESPACE_END
