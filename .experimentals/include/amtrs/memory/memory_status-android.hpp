/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__memory_status__android__hpp
#define	__libamtrs__memory__memory_status__android__hpp
#include <malloc.h>
#include "../java/java/lang/Runtime.hpp"
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

class	memory_status
{
	using	Runtime	= java::classes::java::lang::Runtime;
public:
	memory_status()
	{
	}

	size_t using_size()
	{
		return	mallinfo().uordblks;
	}

	//! Android
	void gc()
	{
		Runtime::find().getRuntime().gc();
	}

protected:
};


AMTRS_NAMESPACE_END
#endif
