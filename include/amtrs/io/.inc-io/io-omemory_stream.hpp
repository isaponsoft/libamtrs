/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io_omemory_stream__hpp
#define	__libamtrs__io_omemory_stream__hpp
AMTRS_IOSTREAM_NAMESPACE_BEGIN


template<class Allocator = std::allocator<char>>
class	basic_omemory_stream
		: public streamif_base
		, protected Allocator
{
public:
	using	allocator_type		= Allocator;
	using	allocator_traits	= std::allocator_traits<allocator_type>;

	basic_omemory_stream() = default;
	basic_omemory_stream(basic_omemory_stream const&) = delete;
	basic_omemory_stream(basic_omemory_stream&& _r)
		: mBuffer(_r.mBuffer)
	{
		_r.mBuffer			= nullptr;
		_r.mAllocateSize	= 0;
		_r.mSize			= 0;
		_r.mPcount			= 0;
		_r.mStatus			= std::ios::goodbit;
	}

	~basic_omemory_stream()
	{
		if (mBuffer)
		{
			allocator_type::deallocate(mBuffer, mAllocateSize);
		}
	}

	basic_omemory_stream& write(void const* _buff, size_t _size)
	{
		size_t	s	= mSize + _size;
		if (s > mAllocateSize)
		{
			size_t	as	= s + (s >> 1);
			char*	nb	= allocator_type::allocate(as);
			std::copy_n(mBuffer, mSize, nb);
			allocator_type::deallocate(mBuffer, mAllocateSize);
			mBuffer			= nb;
			mAllocateSize	= as;
		}
		std::copy_n(reinterpret_cast<char const*>(_buff), _size, mBuffer + mSize);
		mSize	= s;
		mPcount	= _size;
		return	*this;
	}

	size_t pcount() const noexcept
	{
		return	mPcount;
	}

	char* release() noexcept
	{
		char*	r	= mBuffer;
		mBuffer			= nullptr;
		mAllocateSize	= 0;
		mSize			= 0;
		return	r;
	}

	char const* data() const noexcept
	{
		return	mBuffer;
	}

	size_t size() const noexcept
	{
		return	mSize;
	}

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == std::ios::goodbit; }
	bool eof()  const noexcept { return rdstate() & std::ios::eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & std::ios::failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & std::ios::badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept { mStatus = _state; }

private:
	char*		mBuffer			= nullptr;
	size_t		mAllocateSize	= 0;
	size_t		mSize			= 0;
	size_t		mPcount			= 0;
	iostate		mStatus			= std::ios::goodbit;
};

using	omemory_stream	= basic_omemory_stream<std::allocator<char>>;


AMTRS_IOSTREAM_NAMESPACE_END
#endif
