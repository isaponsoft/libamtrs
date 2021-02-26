/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__hash__hash_stream__hpp
#define	__libamtrs__hash__hash_stream__hpp
#include <functional>
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<class...>
class	hash_stream;


template<class FirstIt, class EndIt>
class	hash_stream<FirstIt, EndIt>
{
public:
	using	hash_type		= size_t;
	using	iterator		= FirstIt;
	using	end_iterator	= EndIt;

	constexpr hash_stream(iterator _begin, end_iterator _end) noexcept
		: mBegin(_begin)
		, mCursor(_begin)
		, mEnd(_end)
		, mHash(0)
	{}


	constexpr hash_type operator () () noexcept
	{
		update();
		return	mHash;
	}


protected:
	constexpr void update() noexcept
	{
		mHash	= mHash * (31) + *mCursor++;
		if (mCursor == mEnd)
		{
			mCursor	= mBegin;
		}
	}

	iterator		mBegin;
	iterator		mCursor;
	end_iterator	mEnd;
	hash_type		mHash;
};

AMTRS_NAMESPACE_END
#endif
