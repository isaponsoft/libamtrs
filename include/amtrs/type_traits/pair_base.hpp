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
#ifndef __libamtrs__type_traits__pair_base__hpp
#define __libamtrs__type_traits__pair_base__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN


template<class First, class Second>
struct	pair_base
{
	struct	type
			: First
			, Second
	{
		type() = default;

		template<class A, class B>
		type(A&& _a, B&& _b)
			: First(_a)
			, Second(_b)
		{}
	};

	using	first_type	= First;
	using	second_type	= Second;
};


template<class First>
struct	pair_base<First, void>
{
	using	type		= First;
	using	first_type	= First;
	using	second_type	= void;
};

template<class Second>
struct	pair_base<void, Second>
{
	using	type		= Second;
	using	first_type	= void;
	using	second_type	= Second;
};

template<class First, class Second>
using	pair_base_t	= typename pair_base<First, Second>::type;



AMTRS_NAMESPACE_END
#endif
