/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__streamif__hpp
#define	__libamtrs__io__streamif__hpp
#include <ios>
#include <sstream>
#include <iostream>
AMTRS_IO_NAMESPACE_BEGIN


template<class...>
struct	streamif_traits;


template<class Source, class Elm, class Traits = streamif_traits<Source, Elm>>
class	basic_streamif;


template<class Source, class Value = streamif_traits<Source, char>>
using	streamif	= basic_streamif<Source, char, Value>;

template<class Source, class Value = streamif_traits<Source, char>>
using	wstreamif	= basic_streamif<Source, wchar_t, Value>;


template<class Base, class Derived>
using	base_if	= std::enable_if_t<std::is_base_of<Base, Derived>::value, int>;


template<class Source, class Elm, class T = Source>
auto make_basic_streamif(Source _value)
{
	return	basic_streamif<Source, Elm, streamif_traits<T, Elm>>(std::move(_value));
}


template<class... Source>
auto make_streamif(basic_streamif<Source...> _value) -> basic_streamif<Source...>
{
	return	_value;
}


struct	streamif_base
{
	using	iostate 	= unsigned int;
	static constexpr iostate	goodbit		= std::ios_base::goodbit;
	static constexpr iostate	badbit		= std::ios_base::badbit;
	static constexpr iostate	eofbit		= std::ios_base::eofbit;
	static constexpr iostate	failbit		= std::ios_base::failbit;
	static constexpr iostate	againbit	= 1<<16;

	using	seekdir		= std::ios_base::seekdir;
	static constexpr seekdir	beg			= std::ios_base::beg;
	static constexpr seekdir	cur			= std::ios_base::cur;
	static constexpr seekdir	end			= std::ios_base::end;
};



template<class Source, class Elm, class Traits>
class	basic_streamif
		: public streamif_base
		, public Traits
{
public:
	using	traits_type	= Traits;
	using	value_type	= typename traits_type::value_type;
	using	char_type	= Elm;
	using	fpos_type	= std::fpos<typename std::char_traits<Elm>::state_type>;
	using	pos_type	= std::fpos<typename std::char_traits<Elm>::state_type>;
	using	int_type	= typename std::char_traits<Elm>::int_type;
	using	size_type	= std::streamsize;
	using	iostate		= streamif_base::iostate;

	basic_streamif() = default;
	basic_streamif(basic_streamif const&) = delete;
	basic_streamif& operator = (basic_streamif const&) = delete;
	basic_streamif& operator = (basic_streamif&&) = delete;

	template<class S>
	basic_streamif(S&& _source)
		: mSource(std::move(_source))
		, traits_type(initparam())
	{}

	basic_streamif(basic_streamif&& _r)
		: mSource(std::move(_r.mSource))
		, traits_type(initparam())
	{}

	~basic_streamif()
	{
		// move かつ close() を持っている場合のみ閉じる
		if constexpr ((std::is_same<value_type, Source>::value || std::is_base_of<value_type, Source>::value) && _close)
		{
			traits_type::close(value());
		}
	}


	basic_streamif& read(char_type* _data, size_type _size)
	{
		size_type	s	= 0;
		mGCount	= 0;
		if (good())
		{
			if constexpr (_read)
			{
				auto r = traits_type::read(value(), s, _data, _size);
				mGCount	= s;
				setstate(r);
			}
			else
			{
				setstate(streamif_base::badbit);
			}
		}
		return	*this;
	}

	basic_streamif& write(char_type const* _data, size_type _size)
	{
		size_type	s	= 0;
		mPCount	= 0;
		if (good())
		{
			if constexpr (_write)
			{
				auto r = traits_type::write(value(), s, _data, _size);
				mPCount	= s;
				setstate(r);
			}
			else
			{
				setstate(streamif_base::badbit);
			}
		}
		return	*this;
	}

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == streamif_base::goodbit; }
	bool eof()  const noexcept { return rdstate() & streamif_base::eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & streamif_base::failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & streamif_base::badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept
	{
		mStatus = _state; 
		if constexpr (_clear)
		{
			traits_type::clear(value(), mStatus);
		}
	}

	size_type gcount() const noexcept { return mGCount; }
	size_type pcount() const noexcept { return mPCount; }

	basic_streamif& seek(fpos_type _pos) { return seek(_pos, beg); }
	basic_streamif& seek(fpos_type _pos, seekdir _dir)
	{
		if (good())
		{
			if constexpr (_seek)
			{
				if (auto r = traits_type::seek(value(), _pos, _dir); !((int)r & (int)streamif_base::goodbit))
				{
					setstate(r);
				}
			}
			else
			{
				setstate(streamif_base::badbit);
			}
		}
		return	*this;
	}

	fpos_type tell()
	{
		fpos_type	pos	= 0;
		if (good())
		{
			if constexpr (_tell)
			{
				if (auto r = traits_type::tell(value(), pos); !((int)r & (int)streamif_base::goodbit))
				{
					setstate(r);
				}
			}
			else
			{
				setstate(streamif_base::badbit);
			}
		}

		return	pos;
	}

	size_type size()
	{
		size_type	s	= 0;
		if constexpr (_size)
		{
			if (auto r = traits_type::size(value(), s); r != streamif_base::goodbit)
			{
				setstate(r);
			}
		}
		else if constexpr (_seek && _tell)
		{
			fpos_type	current;
			current	= tell();
			seek(0, end);
			s		= static_cast<size_type>(tell());
			seek(current);
		}
		else
		{
			setstate(streamif_base::badbit);
		}
		return	s;
	}

protected:
	value_type& value()
	{
		if constexpr (std::is_same<value_type, Source>::value || std::is_base_of<value_type, Source>::value)
		{
			return	mSource;
		}
		else
		{
			return	*mSource;
		}
	}

	Source			mSource;
	iostate			mStatus	= streamif_base::goodbit;
	size_type		mGCount	= 0;
	size_type		mPCount	= 0;

private:
	auto initparam()
	{
		if constexpr (std::is_pointer<Source>::value)
		{
			return	mSource;
		}
		else
		{
			return	&mSource;
		}
	}


	template<class TestT>
	static constexpr auto has_close(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::close(std::declval<value_type&>()), std::true_type{});
	static constexpr auto has_close(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_seek(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::seek(std::declval<value_type&>(), std::declval<fpos_type&>(), streamif_base::beg), std::true_type{});
	static constexpr auto has_seek(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_tell(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::tell(std::declval<value_type&>(), std::declval<fpos_type&>()), std::true_type{});
	static constexpr auto has_tell(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_size(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::size(std::declval<value_type&>(), std::declval<size_type&>()), std::true_type{});
	static constexpr auto has_size(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_read(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::read(std::declval<value_type&>(), std::declval<size_type&>(), std::declval<char_type*>(), std::declval<size_type&>()), std::true_type{});
	static constexpr auto has_read(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_write(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::write(std::declval<value_type&>(), std::declval<size_type&>(), std::declval<char_type const*>(), std::declval<size_type&>()), std::true_type{});
	static constexpr auto has_write(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_clear(TestT& v) -> decltype((decltype(std::declval<basic_streamif*>()->TestT::clear(std::declval<value_type&>(), std::declval<iostate&>()))*)nullptr, std::true_type{});
	static constexpr auto has_clear(...) -> std::false_type;

	static constexpr decltype(has_close(std::declval<Traits&>())())	_close	= decltype(has_close(std::declval<Traits&>()))::value;
	static constexpr decltype(has_seek(std::declval<Traits&>())())	_seek	= decltype(has_seek(std::declval<Traits&>()))::value;
	static constexpr decltype(has_tell(std::declval<Traits&>())())	_tell	= decltype(has_tell(std::declval<Traits&>()))::value;
	static constexpr decltype(has_size(std::declval<Traits&>())())	_size	= decltype(has_size(std::declval<Traits&>()))::value;
	static constexpr decltype(has_read(std::declval<Traits&>())())	_read	= decltype(has_read(std::declval<Traits&>()))::value;
	static constexpr decltype(has_write(std::declval<Traits&>())())	_write	= decltype(has_write(std::declval<Traits&>()))::value;
	static constexpr decltype(has_clear(std::declval<Traits&>())())	_clear	= decltype(has_clear(std::declval<Traits&>()))::value;
};




AMTRS_IO_NAMESPACE_END
#endif
