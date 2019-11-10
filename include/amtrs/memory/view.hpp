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
#ifndef	__libamtrs__memory__view__hpp
#define	__libamtrs__memory__view__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class...>
class	view;


template<class T>
class	view<T>
{
public:
	using	size_type	= size_t;
	using	char_type	= T;
	using	view_type	= T;

	view() = default;
	view(const view&) = default;
	view(view&&) = default;
	view& operator = (const view&) = default;
	view& operator = (view&&) = default;


	view(T* _head, size_type _size)
		: mHead(_head)
		, mSize(_size)
	{}

	template<size_t S>
	view(T (&_head)[S])
		: mHead(_head)
		, mSize(S)
	{}


	T* data() const noexcept { return mHead; }
	size_type size() const noexcept { return mSize; }
	bool empty() const noexcept { return size() == 0; }

	auto begin() const noexcept { return mHead; }
	auto end() const noexcept { return mHead + mSize; }

	bool operator == (const view& _r) { return mHead == _r.mHead && mSize == _r.mSize; }
	bool operator != (const view& _r) { return !(*this == _r); }

private:
	T*			mHead;
	size_type	mSize;
};


template<class T, size_t S>
view<T> make_view(T (&_buff)[S])
{
	return	view<T>(_buff);
}


template<class T>
view<T> make_view(T* _buff, size_t _size)
{
	return	view<T>(_buff, _size);
}


AMTRS_NAMESPACE_END
#endif
