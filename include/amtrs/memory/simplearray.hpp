/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__core__simplearray__hpp
#define	__libamtrs__core__simplearray__hpp
#include <algorithm>
#include <initializer_list>
#include <type_traits>
#include "../typeutil/typetraits.hpp"
#include "view.hpp"
AMTRS_NAMESPACE_BEGIN

// とても単純で小さい実装のコピー可能な配列
//	Terminate == true の場合、最後尾に {0} を用意します。
template<class T, bool Terminate = std::is_scalar<T>::value>
class	simplearray
{
	struct	piecewise {};
	using	element_type			= typename std::remove_const<T>::type;
public:
	template<class P>
	class	reverse_iterator_base;

	using	value_type				= T;
	using	reference				= T&;
	using	const_reference			= typename std::add_const<T>::type&;
	using	pointer					= T*;
	using	const_pointer			= typename std::add_const<T>::type*;
	using	size_type				= size_t;
	using	iterator				= pointer;
	using	const_iterator			= const_pointer;
	using	reverse_iterator		= reverse_iterator_base<pointer>;
	using	const_reverse_iterator	= reverse_iterator_base<const_pointer>;

	simplearray() = default;
	simplearray(size_type _size);
	simplearray(simplearray const& r);
	simplearray(simplearray&& r) { swap(r); }

	template<class E>	simplearray(std::initializer_list<E> _init);
	template<class It>	simplearray(It _first, It _last);
	template<class R>	simplearray(R&& _v, std::enable_if_t<can_copy_n<R, value_type>>** = 0);

	~simplearray() { clear(); }

	auto operator = (simplearray const& r) -> simplearray&;
	auto operator = (simplearray&& r) -> simplearray& { clear(); swap(r); return *this; }


	// iterators
	auto begin() noexcept -> iterator { return mList; }
	auto end() noexcept -> iterator { return empty() ? nullptr : mList + mSize; }
	auto begin() const noexcept -> const_iterator { return cbegin(); }
	auto end() const noexcept -> const_iterator { return cend(); }
	auto cbegin() const noexcept -> const_iterator { return mList; }
	auto cend() const noexcept -> const_iterator { return empty() ? nullptr : mList + mSize; }

	auto rbegin() noexcept -> reverse_iterator { return mList + mSize - 1; }
	auto rend() noexcept -> reverse_iterator { return mList - 1; }
	auto rbegin() const noexcept -> const_reverse_iterator { return cbegin(); }
	auto rend() const noexcept -> const_reverse_iterator { return cend(); }
	auto crbegin() const noexcept -> const_reverse_iterator { return mList + mSize - 1; }
	auto crend() const noexcept -> const_reverse_iterator { return mList - 1; }

	// capacity
	auto size() const noexcept -> size_type{ return mSize; }
	bool empty() const noexcept { return size() == 0; }

	// element access
	auto data() noexcept -> pointer{ return mList; }
	auto data() const noexcept -> const_pointer{ return mList; }
	auto front() -> reference{ return mList[0]; }
	auto front() const -> const_reference { return mList[0]; }
	auto back() -> reference{ return mList[size()-1]; }
	auto back() const -> const_reference { return mList[size()-1]; }
	auto operator [] (size_type i) -> reference { return mList[i]; }
	auto operator [] (size_type i) const -> const_reference { return mList[i]; }

	// modifiers
	void clear();
	void swap(simplearray& r) noexcept { std::swap(mList, r.mList); std::swap(mSize, r.mSize); }

	// modifiers : push_back()
	void push_back(value_type const& _v);
	void push_back(value_type&& _v);
	template<class... Args> void push_back(Args&&... _args);
	template<class It> void push_back(It _first, It _last);

	// modifiers : push_front()
	void push_front(value_type const& _v);
	void push_front(value_type&& _v);
	template<class... Args> void push_front(Args&&... _args);
	template<class It> void push_front(It _first, It _last);

	// modifiers : pop
	void pop_back();
	void pop_front();

	// modifiers : shift
	element_type shift();

	// modifiers : resize
	simplearray& resize(size_type _n);

protected:
	template<class Callback>
	void init(size_type _size, Callback _callback)
	{
		mSize	= 0;
		mList	= allocate(_size);
		if (!mList) { return; }
		for (mSize = 0; mSize < _size; ++mSize)
		{
			_callback(&mList[mSize]);
		}
		if constexpr (Terminate)
		{
			if constexpr (std::is_scalar<element_type>::value)
			{
				mList[mSize]	= (element_type)0;
			}
			else
			{
				new (&mList[mSize]) element_type();
			}
		}
	}

	static element_type* allocate(size_type _s)
	{
		if (_s == 0)
		{
			return	nullptr;
		}
		size_type	as	= sizeof(element_type) * (Terminate ? (_s + 1) : _s);
		char*		mem	= new char[as];
		return		reinterpret_cast<element_type*>(mem);
	}

	size_type		mSize	= 0;
	element_type*	mList	= nullptr;
};



template<class T, bool Terminate>
simplearray<T, Terminate>::simplearray(size_type _size)
{
	init(_size, [](auto* e)
	{
		new (e) element_type ();
	});
}


template<class T, bool Terminate>
simplearray<T, Terminate>::simplearray(simplearray const& _r)
{
	auto	it	= _r.begin();
	init(_r.size(), [&](auto* e)
	{
		new (e) element_type (*it++);
	});
}



template<class T, bool Terminate>
template<class E>
simplearray<T, Terminate>::simplearray(std::initializer_list<E> _init)
{
	auto	it	= _init.begin();
	init(_init.size(), [&](auto* e)
	{
		new (e) element_type (*it++);
	});
}


template<class T, bool Terminate>
template<class It>
simplearray<T, Terminate>::simplearray(It _first, It _last)
{
	init(std::distance(_first, _last), [&](auto* e)
	{
		new (e) element_type (*_first++);
	});
}


template<class T, bool Terminate>
template<class R>
simplearray<T, Terminate>::simplearray(R&& _v, std::enable_if_t<can_copy_n<R, value_type>>**)
{
	auto	it	= _v.begin();
	init(_v.size(), [&](auto* e)
	{
		new (e) element_type (*it++);
	});
}



template<class T, bool Terminate>
void simplearray<T, Terminate>::clear()
{
	if (mList)
	{
		auto*	s	= mList;
		for (size_type i = 0; i < (Terminate ? (mSize + 1) : mSize); ++i)
		{
			s[i].~element_type();
		}
		delete[]	reinterpret_cast<char*>(mList);
		mList	= nullptr;
		mSize	= 0;
	}
}


template<class T, bool Terminate>
auto simplearray<T, Terminate>::operator = (simplearray const& _r) -> simplearray&
{
	clear();
	auto	it	= _r.begin();
	init(_r.size(), [&](auto* e)
	{
		new (e) element_type (*it++);
	});
	return	*this;
}



template<class T, bool Terminate>
void simplearray<T, Terminate>::push_front(value_type const& _v)
{
	auto	siz	= mSize;
	auto*	nl	= allocate(siz + 1);
	new (&nl[0]) element_type(_v);
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (&nl[i+1]) element_type(std::move(mList[i]));
		}
		clear();
	}
	mList	= nl;
	mSize	= siz + 1;
}


template<class T, bool Terminate>
void simplearray<T, Terminate>::push_front(value_type&& _v)
{
	auto	siz	= mSize;
	auto*	nl	= allocate(siz + 1);
	new (&nl[0]) element_type(std::move(_v));
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (&nl[i+1]) element_type(std::move(mList[i]));
		}
		clear();
	}
	mList	= nl;
	mSize	= siz + 1;
}


template<class T, bool Terminate>
template<class... Args>
void simplearray<T, Terminate>::push_front(Args&&... _args)
{
	auto	siz	= mSize;
	auto*	nl	= allocate(siz + 1);
	new (&nl[0]) element_type(std::forward<Args>(_args)...);
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (&nl[i+1]) element_type(std::move(mList[i]));
		}
		clear();
	}
	mList	= nl;
	mSize	= siz + 1;
}


template<class T, bool Terminate>
template<class It>
void simplearray<T, Terminate>::push_front(It _first, It _last)
{
	auto	siz	= mSize + std::distance(_first, _last);
	auto*	nl	= allocate(siz);
	auto*	dst	= nl;
	for (auto i = _first; i != _last; ++i)
	{
		new (dst++) element_type(*i);
	}
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (dst++) element_type(std::move(mList[i]));
		}
		clear();
	}
	mList	= nl;
	mSize	= siz;
}




template<class T, bool Terminate>
void simplearray<T, Terminate>::push_back(value_type const& _v)
{
	auto	siz	= mSize;
	auto*	nl	= allocate(siz + 1);
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (&nl[i]) element_type(std::move(mList[i]));
		}
		clear();
	}
	new (&nl[siz]) element_type(_v);
	mList	= nl;
	mSize	= siz + 1;
}


template<class T, bool Terminate>
void simplearray<T, Terminate>::push_back(value_type&& _v)
{
	auto	siz	= mSize;
	auto*	nl	= allocate(siz + 1);
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (&nl[i]) element_type(std::move(mList[i]));
		}
		clear();
	}
	new (&nl[siz]) element_type(std::move(_v));
	mList	= nl;
	mSize	= siz + 1;
}

template<class T, bool Terminate>
template<class... Args>
void simplearray<T, Terminate>::push_back(Args&&... _args)
{
	auto	siz	= mSize;
	auto*	nl	= allocate(siz + 1);
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (&nl[i]) element_type(std::move(mList[i]));
		}
		clear();
	}
	new (&nl[siz]) element_type(std::forward<Args>(_args)...);
	mList	= nl;
	mSize	= siz + 1;
}


template<class T, bool Terminate>
template<class It>
void simplearray<T, Terminate>::push_back(It _first, It _last)
{
	auto	ext	= std::distance(_first, _last);
	auto	siz	= mSize + ext;
	auto*	nl	= allocate(siz);
	auto*	dst	= nl;
	if (mList)
	{
		for (size_type i = 0; i < mSize; ++i)
		{
			new (dst++) element_type(std::move(mList[i]));
		}
		clear();
	}
	for (auto i = _first; i != _last; ++i)
	{
		new (dst++) element_type(*i);
	}
	mList	= nl;
	mSize	= siz;
}


template<class T, bool Terminate>
void simplearray<T, Terminate>::pop_back()
{
	if (empty())
	{
		return;
	}

	if (size() == 1)
	{
		clear();
		return;
	}

	auto	siz	= mSize - 1;
	auto*	nl	= allocate(siz);
	for (size_type i = 0; i < siz; ++i)
	{
		new (&nl[i]) element_type(std::move(mList[i]));
	}
	clear();
	mList	= nl;
	mSize	= siz;
}


template<class T, bool Terminate>
void simplearray<T, Terminate>::pop_front()
{
	if (empty())
	{
		return;
	}

	if (size() == 1)
	{
		clear();
		return;
	}

	auto	siz	= mSize - 1;
	auto*	nl	= allocate(siz);
	for (size_type i = 1; i < mSize; ++i)
	{
		new (&nl[i-1]) element_type(std::move(mList[i]));
	}
	clear();
	mList	= nl;
	mSize	= siz;
}

template<class T, bool Terminate>
auto simplearray<T, Terminate>::shift() -> element_type
{
	if (empty())
	{
		return	{};
	}

	if (size() == 1)
	{
		element_type	r(std::move(mList[0]));
		clear();
		return	r;
	}

	auto			siz	= mSize - 1;
	auto*			nl	= allocate(siz);
	element_type	r(std::move(mList[0]));
	for (size_type i = 0; i < siz; ++i)
	{
		new (&nl[i]) element_type(std::move(mList[i + 1]));
	}
	clear();
	mList	= nl;
	mSize	= siz;
	return	r;
}


template<class T, bool Terminate>
auto simplearray<T, Terminate>::resize(size_type _n) -> simplearray&
{
	if (_n != size())
	{
		if (_n > 0)
		{
			auto*			nl	= allocate(_n);
			auto			dst	= nl;
			auto			end	= dst + _n;
			if (!empty())
			{
				auto			cs	= _n < mSize ? _n : mSize;
				size_type		i	= 0;
				for (; i < cs; ++i)
				{
					new (&dst[i]) element_type(std::move(mList[i]));
				}
				dst	+= cs;
				clear();
			}
			for (; dst != end; ++dst)
			{
				new (dst) element_type ();
			}
			mList	= nl;
			mSize	= _n;
		}
		else
		{
			// resize to 0.
			clear();
		}
	}
	return	*this;
}


template<class T, bool Terminate>
template<class P>
class	simplearray<T, Terminate>::reverse_iterator_base
{
public:
	using	pointer		= P;

	reverse_iterator_base() = default;
	reverse_iterator_base(reverse_iterator_base const&) = default;
	reverse_iterator_base& operator = (reverse_iterator_base const&) = default;
	reverse_iterator_base(pointer _p) : mPointer(_p) {}
	bool operator == (reverse_iterator_base const& r) const noexcept { return mPointer == r.mPointer; }
	bool operator != (reverse_iterator_base const& r) const noexcept { return mPointer != r.mPointer; }
	auto& operator * () noexcept { return *mPointer; }
	auto* operator -> () noexcept { return mPointer; }
	reverse_iterator_base& operator ++ () noexcept { --mPointer; return *this; }
	reverse_iterator_base operator ++ (int) noexcept { reverse_iterator_base r(*this); --mPointer; return r; }
	reverse_iterator_base& operator -- () noexcept { ++mPointer; return *this; }
	reverse_iterator_base operator -- (int) noexcept { reverse_iterator_base r(*this); ++mPointer; return r; }
	auto operator - (reverse_iterator_base const& r) { return mPointer - r.mPointer; }
protected:
	pointer	mPointer	= nullptr;
};


AMTRS_NAMESPACE_END
#endif
