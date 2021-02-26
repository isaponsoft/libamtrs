/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__memory__bits_view__hpp
#define	__libamtrs__memory__bits_view__hpp
#include "def.hpp"
AMTRS_NAMESPACE_BEGIN

template<class CharT = char>
class	basic_bits_view;


template<class CharT = char>
class	bits_view_altanative
{
public:
	using	pointer			= CharT*;
	using	off_type		= size_t;

	bits_view_altanative(pointer _pointer, off_type _offset)
		: mPointer(_pointer)
		, mBitOffset(_offset)
	{}

	operator bool () const noexcept { return (*mPointer >> (mBitOffset)) & 0x01; }

	bits_view_altanative const& operator = (bool _bit) const noexcept
	{
		if (_bit)
		{
			*mPointer |= 1<<(mBitOffset);
		}
		else
		{
			*mPointer &= ~(1<<(mBitOffset));
		}
		return	*this;
	}

protected:
	pointer		mPointer;
	off_type	mBitOffset;
};


template<class CharT>
class	bits_view_iterator
{
public:
	using	pointer			= CharT*;
	using	const_pointer	= typename std::add_const<CharT>::type*;
	using	off_type		= intptr_t;

	bits_view_iterator() = default;
	bits_view_iterator(const bits_view_iterator&) = default;
	bits_view_iterator& operator = (const bits_view_iterator&) = default;

	bits_view_altanative<CharT> operator * () const noexcept { return bits_view_altanative<CharT>(mPointer, mBitOffset); }

	bool operator == (const bits_view_iterator& _r) const noexcept { return mPointer == _r.mPointer && mBitOffset == _r.mBitOffset; }
	bool operator != (const bits_view_iterator& _r) const noexcept { return !(*this == _r); }
	off_type operator - (const bits_view_iterator& _r) const noexcept { return off_type(mPointer - _r.mPointer) * sizeof(CharT) + off_type(mBitOffset - _r.mBitOffset); }

	bits_view_iterator& operator ++ () noexcept
	{
		++mBitOffset;
		mPointer	+= mBitOffset / (sizeof(CharT) * 8);
		mBitOffset	&= (sizeof(CharT) * 8 - 1);
		return	*this;
	}

	bits_view_iterator& operator += (off_type _off) noexcept
	{
		mBitOffset	+= _off;
		mPointer	+= mBitOffset / (sizeof(CharT) * 8);
		mBitOffset	&= (sizeof(CharT) * 8 - 1);
		return	*this;
	}

#if	1
	int read(size_t _bitSize) noexcept
	{
		int		retval	= 0;
		auto	it		= *this;
		int		i		= 0;
		while (_bitSize > 0)
		{
			if (*it)
			{
				retval	|= 1<<i;
			}
			++it;
			++i;
			--_bitSize;
		}
		return	retval;
	}
#else
	int read(size_t _bitSize) noexcept
	{
		int		retval	= 0;
		auto	it		= *this;
		while (_bitSize > 0)
		{
			retval	= (retval << 1) + (*it ? 1 : 0);
			++it;
			--_bitSize;
		}
		return	retval;
	}
#endif

protected:
	bits_view_iterator(pointer _cur, off_type _off)
		: mPointer(_cur)
		, mBitOffset(_off)
	{}

	pointer		mPointer;
	off_type	mBitOffset;

	template<class>
	friend	class	basic_bits_view;
};


template<class CharT>
class	basic_bits_view
{
public:
	using	value_type		= CharT;
	using	char_type		= bool;
	using	pointer			= value_type const*;
	using	size_type		= size_t;
	using	off_type		= size_t;
	using	pos_type		= size_t;

	using	const_iterator	= bits_view_iterator<CharT const>;
	using	iterator		= const_iterator;


	basic_bits_view() = default;
	basic_bits_view(const basic_bits_view&) = default;
	basic_bits_view& operator = (const basic_bits_view&) = default;

	basic_bits_view(pointer _pointer, size_type _sizeOfBits, off_type _bitOffset = 0)
		: mTop(_pointer + (_bitOffset / (sizeof(value_type)*8)))
		, mOffset(_bitOffset % (sizeof(value_type)*8))
		, mSizeOfBits(_sizeOfBits)
	{}

	size_type size() const noexcept { return mSizeOfBits; }

	iterator begin() noexcept { return iterator(mTop, mOffset); }
	iterator end() noexcept { return iterator(mTop + ((mOffset + mSizeOfBits) / (sizeof(value_type)*8)), ((mOffset + mSizeOfBits) % (sizeof(value_type)*8))); }
	const_iterator begin() const noexcept { return cbegin(); }
	const_iterator end() const noexcept { return cend(); }
	const_iterator cbegin() const noexcept { return iterator(mTop, mOffset); }
	const_iterator cend() const noexcept { return const_iterator(mTop + ((mOffset + mSizeOfBits) / (sizeof(value_type)*8)), ((mOffset + mSizeOfBits) % (sizeof(value_type)*8))); }

private:
	pointer		mTop;
	off_type	mOffset;
	pos_type	mSizeOfBits;
};


using	bit_view	= basic_bits_view<uint8_t>;

AMTRS_NAMESPACE_END
#endif
