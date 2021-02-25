/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__iostream__obuffer__hpp
#define	__libamtrs__iostream__obuffer__hpp
AMTRS_IOSTREAM_NAMESPACE_BEGIN


//! read(char*, std::size_t) を抽象化するために存在するテンプレート
//! 配列やイテレータなどを使ったバッファへのアクセスを抽象化します。
//! std::streambuf に比べて機能が限定的ですが、仮想関数を使用しない分、オーバーヘッドが少なめです。
template<class...>
struct	basic_obuffer;


template<class IteratorT>
struct	basic_obuffer<IteratorT>
{
	static_assert(std::is_base_of<typename std::iterator_traits<IteratorT>::iterator_category, std::random_access_iterator_tag>::value);

	using	iostate		= std::ios_base::iostate;
	using	char_type	= typename std::iterator_traits<IteratorT>::value_type;
	using	iterator	= IteratorT;

	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	basic_obuffer() = default;
	basic_obuffer(const basic_obuffer&) = default;
	basic_obuffer(basic_obuffer&&) = default;
	basic_obuffer& operator = (const basic_obuffer&) = default;
	basic_obuffer& operator = (basic_obuffer&&) = default;

	basic_obuffer(iterator _first, iterator _last)
		: mFirst(_first), mLast(_last), mCursor(_first)
	{}


	basic_obuffer& write(const char_type* _buff, std::streamsize _n)
	{
		auto			cap			= capacity();
		std::streamsize	copySize	= _n;
		if (cap <= copySize)
		{
			copySize	= cap;
			setstate(eofbit);
		}
		std::copy_n(_buff, copySize, mCursor);
		mCursor += copySize;
		return	*this;
	}


	bool good() const noexcept { return rdstate() == 0; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mState; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mState = _state; }

protected:
	std::streamsize capacity() const noexcept { return mLast - mCursor; }

	iterator	mFirst, mLast, mCursor;
	iostate		mState	= goodbit;
};


template<class CharT, class Traits>
struct	basic_obuffer<std::basic_ostream<CharT, Traits>>
{
	using	iostate		= std::ios_base::iostate;
	using	_base_type	= std::basic_ostream<CharT, Traits>;
	using	char_type	= typename _base_type::char_type;

	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	basic_obuffer() = default;
	basic_obuffer(const basic_obuffer&) = default;
	basic_obuffer(basic_obuffer&&) = default;
	basic_obuffer& operator = (const basic_obuffer&) = default;
	basic_obuffer& operator = (basic_obuffer&&) = default;

	basic_obuffer(_base_type& _stream)
		: mStream(_stream)
	{}


	basic_obuffer& write(const char_type* _buff, std::streamsize _n)
	{
		mStream.write(_buff, _n);
		return	*this;
	}

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof(); }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad(); }

	iostate rdstate() const noexcept { return mStream.rdstate(); }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = goodbit) noexcept { mStream.clear(_state); }

protected:
	_base_type&	mStream;
};


template<class CharT, std::size_t N>
struct	basic_obuffer<CharT[N]> : basic_obuffer<CharT*>
{
	using	_base_type	= basic_obuffer<CharT*>;
	using	char_type	= typename _base_type::char_type;

	basic_obuffer(char_type (&_buff)[N])
		: _base_type(std::begin(_buff), std::end(_buff))
	{}

	using	_base_type::operator =;
};



template<class ValueT>
auto make_obuffer(ValueT& _buffer) -> basic_obuffer<ValueT>
{
	return	basic_obuffer<ValueT>(_buffer);
}

template<class IteratorT>
auto make_obuffer(IteratorT _first, IteratorT _last) -> basic_obuffer<IteratorT>
{
	return	basic_obuffer<IteratorT>(_first, _last);
}


AMTRS_IOSTREAM_NAMESPACE_END
#endif
