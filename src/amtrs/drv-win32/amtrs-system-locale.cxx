/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <shlobj.h>
#include <amtrs/system/env.hpp>
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")
AMTRS_SYSTEM_NAMESPACE_BEGIN

sstring get_locale_rfc3282()
{
	CHAR	iso639 [10];
	CHAR	iso3166[10];
	GetLocaleInfoA(GetSystemDefaultLCID(), LOCALE_SISO639LANGNAME , iso639,  sizeof(iso639));
	GetLocaleInfoA(GetSystemDefaultLCID(), LOCALE_SISO3166CTRYNAME, iso3166, sizeof(iso3166));
	return	format("{}-{}", { iso639, iso3166 });
};


AMTRS_SYSTEM_NAMESPACE_END
