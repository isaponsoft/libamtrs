/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__iostream__ibuffer__hpp
#define	__libamtrs__iostream__ibuffer__hpp
AMTRS_IOSTREAM_NAMESPACE_BEGIN


//! ===========================================================================
//! ストリームによる読み込みインターフェースを共通化するテンプレートです。
//! ---------------------------------------------------------------------------
//! ibuff は単純なバイナリの読み取りを抽象化し、配列やイテレータ、
//! basic_istream などに対して同様のインターフェースを提供します。
//! ---------------------------------------------------------------------------
template<class...>
class	ibuff;


//! ===========================================================================
//! ストリームによる読み込みを抽象化するテンプレートです。
//! ---------------------------------------------------------------------------
//! ibuff は単純なバイナリの読み取りを抽象化し、配列やイテレータ、
//! basic_istream などに対して同様のインターフェースを提供します。
//! ---------------------------------------------------------------------------
template<class CharT>
class	ivbuff;



//! ===========================================================================
//! std::basic_istream に対するアクセスを抽象化します。
//! ---------------------------------------------------------------------------
template<class CharT, class TraitsT>
auto make_ibuff(std::basic_istream<CharT, TraitsT>& _in) -> ibuff<std::basic_istream<CharT, TraitsT>>
{
	return	{_in};
}

//! ===========================================================================
//! begin, end を持つランダムイテレータに対するアクセスを抽象化します。
//! ---------------------------------------------------------------------------
template<class IteratorT>
auto make_ibuff(IteratorT _fisrt, IteratorT _last, typename std::iterator_traits<IteratorT>::iterator_category* = 0) -> ibuff<IteratorT>
{
	return	{_fisrt, _last};
}

//! ===========================================================================
//! 配列に対するアクセスを抽象化します。
//! ---------------------------------------------------------------------------
template<class CharT, std::size_t N>
auto make_ibuff(CharT (&_buff)[N]) -> ibuff<CharT*>
{
	return	{std::begin(_buff), std::end(_buff)};
}



template<class CharT>
class	ivbuff
{
public:
	class	object;
	using	char_type		= CharT;
	using	iostate			= std::ios_base::iostate;
	using	pos_type		= std::intmax_t;
	using	off_type		= std::intmax_t;

	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	ivbuff() = default;
	ivbuff(const ivbuff&) = default;
	ivbuff(ivbuff&&) = default;
	ivbuff& operator = (const ivbuff&) = default;
	ivbuff& operator = (ivbuff&&) = default;
	ivbuff(object* _obj) : mObject(_obj) {}

	std::streamsize read(char_type* _dest, std::streamsize _count) { return mObject->read(_dest, _count); }
	pos_type tellg() { return mObject->tellg(); }
	ivbuff& seekg(off_type _off, std::ios_base::seekdir _dir) { mObject->seekg(_off, _dir); return *this; }
	bool good() const noexcept { return mObject->good; }
	bool eof()  const noexcept { return mObject->eof; }
	bool fail() const noexcept { return mObject->fail; }
	bool bad()  const noexcept { return mObject->bad; }
	iostate rdstate() const noexcept { return mObject->rdstate(); }
	void setstate(iostate _state) noexcept { mObject->setstate(_state); }
	void clear(iostate _state = goodbit) noexcept { mObject->clear(_state); }

	class	object
			: public ref_object
	{
	public:
		virtual std::streamsize read(char_type* _dest, std::streamsize _count) = 0;
		virtual pos_type tellg() = 0;
		virtual void seekg(off_type _off, std::ios_base::seekdir _dir) = 0;
		virtual bool good() const noexcept = 0;
		virtual bool eof()  const noexcept = 0;
		virtual bool fail() const noexcept = 0;
		virtual bool bad()  const noexcept = 0;
		virtual iostate rdstate() const noexcept = 0;
		virtual void setstate(iostate _state) noexcept = 0;
		virtual void clear(iostate _state = goodbit) noexcept = 0;
	};
protected:
	ref<object>	mObject;
};


//! ===========================================================================
//! ibuff を仮想関数により抽象化します。
//! ---------------------------------------------------------------------------
template<class... Types>
auto make_ivbuff(ibuff<Types...>& _in) -> ivbuff<typename ibuff<Types...>::char_type>
{
	struct	buff : ivbuff<typename ibuff<Types...>::char_type>::object
	{
		using	char_type		= typename ibuff<Types...>::char_type;
		using	iostate			= std::ios_base::iostate;
		using	pos_type		= std::intmax_t;
		using	off_type		= std::intmax_t;

		buff(ibuff<Types...>&	_buff) : mBuff(_buff) {}

		virtual std::streamsize read(char_type* _dest, std::streamsize _count) override { return mBuff.read(_dest, _count); }
		virtual pos_type tellg() override  { return static_cast<pos_type>(mBuff.tellg()); }
		virtual void seekg(off_type _off, std::ios_base::seekdir _dir) override { mBuff.seekg(_off, _dir); }
		virtual bool good() const noexcept override { return mBuff.good(); }
		virtual bool eof()  const noexcept override { return mBuff.eof(); }
		virtual bool fail() const noexcept override { return mBuff.fail(); }
		virtual bool bad()  const noexcept override { return mBuff.bad(); }
		virtual iostate rdstate() const noexcept override { return mBuff.rdstate(); }
		virtual void setstate(iostate _state) noexcept override { return mBuff.setstate(_state); }
		virtual void clear(iostate _state) noexcept override { return mBuff.clear(_state); }

		ibuff<Types...>&	mBuff;
	};

	return	{new buff(_in)};
}



//! ===========================================================================
//! std::basic_istream<> に対するアクセスを抽象化します。
//! ---------------------------------------------------------------------------
template<class CharT, class TraitsT>
class	ibuff<std::basic_istream<CharT, TraitsT>>
{
protected:
	using	stream_type		= std::basic_istream<CharT, TraitsT>;
public:
	using	char_type		= CharT;
	using	iostate			= std::ios_base::iostate;
	using	pos_type		= typename stream_type::pos_type;
	using	off_type		= typename stream_type::off_type;

	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;


	ibuff(stream_type& _stream)
		: mStream(_stream)
	{}

	ibuff(ibuff&& _stream) = default;
	ibuff& operator = (ibuff&& _stream) = default;

	std::streamsize read(char_type* _dest, std::streamsize _count)
	{
		mStream.read(_dest, _count);
		return mStream.gcount();
	}

	pos_type tellg()
	{
		return	mStream.tellg();
	}

	ibuff& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		mStream.seekg(_off, _dir);
		return	*this;
	}

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof();  }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad();  }

	iostate rdstate() const noexcept { return mStream.rdstate(); }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = goodbit) noexcept { mStream.clear(_state); }

protected:
	stream_type&	mStream;
};



template<class IteratorT>
class	ibuff<IteratorT>
{
	static_assert(std::is_base_of<typename std::iterator_traits<IteratorT>::iterator_category, std::random_access_iterator_tag>::value);
public:
	using	iostate		= std::ios_base::iostate;
	using	char_type	= typename std::iterator_traits<IteratorT>::value_type;
	using	iterator	= IteratorT;
	using	pos_type	= typename std::iterator_traits<IteratorT>::difference_type;
	using	off_type	= typename std::iterator_traits<IteratorT>::difference_type;

	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	ibuff() = default;
	ibuff(const ibuff&) = default;
	ibuff(ibuff&&) = default;
	ibuff& operator = (const ibuff&) = default;
	ibuff& operator = (ibuff&&) = default;

	ibuff(iterator _first, iterator _last)
		: mFirst(_first), mLast(_last), mCursor(0)
	{}


	std::streamsize read(char_type* _buff, std::streamsize _n)
	{
		auto			cap			= capacity();
		std::streamsize	copySize	= _n;
		if (cap <= copySize)
		{
			copySize	= cap;
			setstate(eofbit);
		}
		std::copy_n(mFirst + mCursor, copySize, _buff);
		mCursor += copySize;
		return	copySize;
	}

	pos_type tellg()
	{
		return	mCursor;
	}

	ibuff& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		switch (_dir)
		{
			case std::ios_base::beg:
			{
				mCursor = _off;
				break;
			}

			case std::ios_base::cur:
			{
				mCursor += _off;
				break;
			}

			case std::ios_base::end:
			{
				mCursor = (mLast - mFirst) - _off;
				break;
			}

			default: break;
		}
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
	std::streamsize capacity() const noexcept { return (mLast - mFirst) - mCursor; }

	iterator	mFirst, mLast;
	pos_type	mCursor;
	iostate		mState	= goodbit;
};





AMTRS_IOSTREAM_NAMESPACE_END
#endif
