/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__basic__bucket__hpp
#define	__libamtrs__basic__bucket__hpp
AMTRS_NAMESPACE_BEGIN


template<class...>
class	bucket;			// Prototype. not use.


// ****************************************************************************
//! begin(), end() を抽象化するデータ構造。
// ----------------------------------------------------------------------------
template<class Iterator>
auto make_bucket(Iterator _first, Iterator _last) -> bucket<Iterator>
{
	return	bucket<Iterator>(_first, _last);
}


// ****************************************************************************
//! フィルター付きの begin(), end() を抽象化するデータ構造。
// ----------------------------------------------------------------------------
template<class Iterator, class Filter>
auto make_bucket(Iterator _first, Iterator _last, Filter&& _func) -> bucket<Iterator, Filter>
{
	return	bucket<Iterator, Filter>(_first, _last, std::forward<Filter>(_func));
}

// ****************************************************************************
//! 配列とサイズを抽象化するデータ構造。
// ----------------------------------------------------------------------------
template<class T>
auto make_bucket(T* _buffer, std::size_t _size) -> bucket<T*>
{
	return	make_bucket<T*>(_buffer, _buffer + _size);
}



template<class Iterator, class Owner>
class	bucket_iterator
{
public:
	using	value_type			= typename std::iterator_traits<Iterator>::value_type;
	using	difference_type		= typename std::iterator_traits<Iterator>::difference_type;
	//using	size_type			= typename std::iterator_traits<Iterator>::size_type;
	using	pointer 			= typename std::iterator_traits<Iterator>::pointer;
	using	reference  			= typename std::iterator_traits<Iterator>::reference;
	using	iterator_category	= typename std::iterator_traits<Iterator>::iterator_category;

	constexpr bucket_iterator() = default;
	constexpr bucket_iterator(const bucket_iterator&) = default;
	constexpr bucket_iterator(bucket_iterator&&) = default;
	constexpr bucket_iterator& operator = (const bucket_iterator&) = default;
	constexpr bucket_iterator& operator = (bucket_iterator&&) = default;

	constexpr bucket_iterator(Owner* _owner, Iterator _it)
		: mOwner(_owner)
		, mIterator(_it)
	{}

	constexpr auto  operator * () const noexcept -> decltype(std::declval<Owner&>().mFunc(*std::declval<Iterator&>())) { return mOwner->mFunc(*mIterator); }
	constexpr bucket_iterator& operator ++ () noexcept { ++mIterator; return *this; }
	constexpr bucket_iterator  operator ++ (int) noexcept { bucket_iterator retval(*this); ++(*this); return retval; }
	constexpr bucket_iterator& operator -- () noexcept { --mIterator; return *this; }
	constexpr bucket_iterator  operator -- (int) noexcept { bucket_iterator retval(*this); --(*this); return retval; }
	constexpr bucket_iterator& operator += (difference_type _c) noexcept { mIterator += _c; return *this; }
	constexpr bucket_iterator& operator -= (difference_type _c) noexcept { mIterator -= _c; return *this; }
	constexpr bool operator == (const bucket_iterator& _rhs) const noexcept { return mIterator == _rhs.mIterator; }
	constexpr bool operator != (const bucket_iterator& _rhs) const noexcept { return mIterator != _rhs.mIterator; }
	constexpr bool operator <= (const bucket_iterator& _rhs) const noexcept { return mIterator <= _rhs.mIterator; }
	constexpr bool operator <  (const bucket_iterator& _rhs) const noexcept { return mIterator <  _rhs.mIterator; }
	constexpr difference_type operator - (const bucket_iterator& _rhs) const noexcept { return mIterator - _rhs.mIterator; }

protected:
	Owner*		mOwner;
	Iterator	mIterator;
};


template<class Iterator>
class	bucket<Iterator>
{
public:
	using	iterator	= Iterator;


	bucket() = delete;
	bucket(const bucket&) = delete;
	bucket(bucket&&) = default;
	bucket& operator = (const bucket&) = delete;
	bucket& operator = (bucket&&) = default;

	template<class I>
	bucket(I&& _first, I&& _last)
		: mFirst(std::forward<I>(_first))
		, mLast(std::forward<I>(_last))
	{}

	auto& top() noexcept { return *mFirst; }
	iterator begin() noexcept { return mFirst; }
	iterator end()   noexcept { return mLast ; }
	auto size() const noexcept { return std::distance(mFirst, mLast); }
	bool empty() const noexcept { return size() == 0; }

protected:
	Iterator	mFirst;
	Iterator	mLast;
};


template<class Iterator, class Filter>
class	bucket<Iterator, Filter>
{
public:
	using	iterator	= bucket_iterator<Iterator, bucket>;


	bucket() = delete;
	bucket(const bucket&) = delete;
	bucket(bucket&&) = default;
	bucket& operator = (const bucket&) = delete;
	bucket& operator = (bucket&&) = default;

	template<class I, class F>
	bucket(I&& _first, I&& _last, F&& _func)
		: mFirst(std::forward<I>(_first))
		, mLast(std::forward<I>(_last))
		, mFunc(std::forward<F>(_func))
	{}

	auto& top() noexcept { return *mFirst; }
	iterator begin() noexcept { return iterator(this, mFirst); }
	iterator end()   noexcept { return iterator(this, mLast ); }
	auto size() const noexcept { return std::distance(mFirst, mLast); }
	bool empty() const noexcept { return size() == 0; }

protected:
	Iterator	mFirst;
	Iterator	mLast;
	Filter		mFunc;

	template<class, class>
	friend	class	bucket_iterator;
};




AMTRS_NAMESPACE_END
#endif
