﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__application__application_freebsd__hpp
#define	__libamtrs__application__application_freebsd__hpp
#include <X11/Xlib.h>
#include <list>
AMTRS_NAMESPACE_BEGIN


inline std::string application::cache_dir() const
{
	return	".";
}


inline std::string application::documents_dir() const
{
	return	{};
}



inline std::string application::files_dir(bool _external) const
{
	return	".";
}


inline void application::foreground()
{
}


AMTRS_NAMESPACE_END
#endif
