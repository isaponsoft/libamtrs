/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
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
