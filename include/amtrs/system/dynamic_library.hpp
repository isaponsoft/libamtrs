/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__system__dynamic_library__hpp
#define	__libamtrs__system__dynamic_library__hpp
#include "../memory/bufferif.h"
#include "../string/text.hpp"
AMTRS_SYSTEM_NAMESPACE_BEGIN

class	dynamic_library
{
public:
	dynamic_library() = default;
	dynamic_library(dynamic_library const&) = delete;
	dynamic_library(dynamic_library&& _r)
		: mHandle(_r.mHandle)
	{
		_r.mHandle	= nullptr;
	}
	dynamic_library& operator = (dynamic_library const&) = delete;
	dynamic_library& operator = (dynamic_library&& _r)
	{
		clear();
		mHandle		= _r.mHandle;
		_r.mHandle	= nullptr;
		return	*this;
	}

	dynamic_library(text _filename, bool _add_ext = true);
	void* get_address(text _symbol);

	void clear();
	bool empty() const noexcept { return mHandle == nullptr; }
	void* release() { void* r = mHandle; mHandle = nullptr; return r; }

private:
	void*	mHandle	= nullptr;
};

AMTRS_SYSTEM_NAMESPACE_END
#endif
