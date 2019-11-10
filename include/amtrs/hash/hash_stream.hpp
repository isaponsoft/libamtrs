/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
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
