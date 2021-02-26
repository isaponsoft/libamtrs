/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__sound__types__hpp
#define	__libamtrs__sound__types__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


struct	sound_format
{
	int			channels;
	int			bitsParSamples;
	int64_t		samplesParSecond;
};


AMTRS_NAMESPACE_END
#endif
