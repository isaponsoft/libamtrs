/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <stdio.h>
#include <stdlib.h>
#include <amtrs/amtrs.hpp>
#include <syslog.h>

AMTRS_LOGGING_NAMESPACE_BEGIN


void write(loglevel _level, const char* _msg, std::size_t _size, bool _return)
{
	char const*	fmt	= _return
					? "%s\n"
					: "%s";
	fprintf(stderr, fmt, _msg);
}


AMTRS_LOGGING_NAMESPACE_END
