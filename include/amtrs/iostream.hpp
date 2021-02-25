/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__iostream__hpp
#define	__libamtrs__iostream__hpp
#include "amtrs.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ios>
#include <iosfwd>

#define	AMTRS_IOSTREAM_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace ios {
#define	AMTRS_IOSTREAM_NAMESPACE_END	} AMTRS_NAMESPACE_END

#include ".inc/iostream-bin.hpp"				// dep
#include ".inc/iostream-rstream.hpp"			// dep
#include ".inc/iostream-wstream.hpp"			// dep
#include ".inc/iostream-fstream.hpp"			// dep
#include ".inc/iostream-ibuffer.hpp"			// dep
#include ".inc/iostream-obuffer.hpp"			// dep
#include ".inc/iostream-viewstream.hpp"			// dep
#endif
