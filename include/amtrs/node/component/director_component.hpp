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
#ifndef	__libamtrs__node__component__director_component__hpp
#define	__libamtrs__node__component__director_component__hpp
#include <map>
#include <string>
#include "../../g3d/@"
AMTRS_NAMESPACE_BEGIN namespace component {

template<class Node, class DirectorT>
class	director_component
{
public:
	using	director_type	= DirectorT;

	const director_type* get_director() const noexcept
	{
		return	mDirector;
	}

	const director_type* set_director(const director_type* _director) noexcept
	{
		mDirector	= _director;
		return	_director;
	}

	static director_type* get_default_director()
	{
		return	default_director();
	}

	static void set_default_director(director_type* _director)
	{
		default_director()	= _director;
	}

private:
	static director_type*& default_director()
	{
		static director_type*	_	= nullptr;
		return	_;
	}

	ref<const director_type>	mDirector;
};


} AMTRS_NAMESPACE_END
#endif
