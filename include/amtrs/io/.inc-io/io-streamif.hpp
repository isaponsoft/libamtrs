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


template<class Source, class Traits = streamif_traits<Source>>
class	basic_streamif;


template<class Source, class Value = streamif_traits<Source>>
using	streamif	= basic_streamif<Source, Value>;


template<class Base, class Derived>
using	base_if	= std::enable_if_t<std::is_base_of<Base, Derived>::value, int>;


template<class Source, class T = Source>
auto make_basic_streamif(Source _value)
{
	return	basic_streamif<Source, streamif_traits<T>>(std::move(_value));
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

	using	seekdir		= AMTRS_IO_NAMESPACE::seekdir;
	static constexpr seekdir	beg			= seekdir_beg;
	static constexpr seekdir	cur			= seekdir_cur;
	static constexpr seekdir	end			= seekdir_end;

	using	pos_type	= std::fpos<typename std::char_traits<char>::state_type>;
	using	size_type	= std::streamsize;
};


template<class T, class O>
struct	__stream_type
{
private:
	template<class S>	static constexpr auto gettype(S&)  -> decltype(typename S::stream_type{});
						static constexpr auto gettype(...) -> std::remove_pointer_t<O>;
public:
	using	type	= decltype(gettype(std::declval<T&>()));
};



/*
 * stream interface. like a iostream proxy.
 */
template<class Source, class Traits>
class	basic_streamif
		: public streamif_base
		, public Traits
{
public:
	using	traits_type	= Traits;
	using	stream_type	= typename __stream_type<traits_type, Source>::type;
	using	pos_type	= streamif_base::pos_type;
	using	fpos_type	= streamif_base::pos_type;
	using	size_type	= streamif_base::size_type;
	using	iostate		= streamif_base::iostate;

	basic_streamif() = default;
	basic_streamif(basic_streamif const&) = delete;
	basic_streamif& operator = (basic_streamif const&) = delete;
	basic_streamif& operator = (basic_streamif&&) = delete;

	template<class... Args>
	basic_streamif(Args&&... _args)
		: mSource(std::forward<Args>(_args)...)
	{
		if constexpr (_init)
		{
			traits_type::init(initparam());
		}
	}

	basic_streamif(basic_streamif&& _r) = default;

	~basic_streamif()
	{
		// move かつ close() を持っている場合のみ閉じる
		if constexpr ((std::is_same<stream_type, Source>::value || std::is_base_of<stream_type, Source>::value) && _close)
		{
			traits_type::close(value());
		}
	}


	basic_streamif& read(void* _data, size_type _size)
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
			// read(), gcount(), rdstate() があるならそのまま。
			else if constexpr (__iotest<stream_type>::has_read_and_gcount && __iotest<stream_type>::has_rdstate)
			{
				value().read(_data, _size);
				mPCount	= value().gcount();
				setstate(value().rdstate());
			}
			else
			{
				//static_assert(0, "read unsupported");
				//setstate(streamif_base::badbit);
			}
		}
		return	*this;
	}

	basic_streamif& write(void const* _data, size_type _size)
	{
		size_type	s	= 0;
		mPCount	= 0;
		if (good())
		{
			// traits::write(), traits::pcount(), traits::rdstate() があるならそのまま。
			if constexpr (_write)
			{
				auto r = traits_type::write(value(), s, _data, _size);
				mPCount	= s;
				setstate(r);
			}
			// write(), pcount(), rdstate() があるならそのまま。
			else if constexpr (__iotest<stream_type>::has_write_and_pcount && __iotest<stream_type>::has_rdstate)
			{
				value().write(_data, _size);
				mPCount	= value().pcount();
				setstate(value().rdstate());
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

	void close()
	{
		if constexpr (_close)
		{
			traits_type::close(value());
		}
	}

	stream_type& stream()
	{
		return	value();
	}

protected:
	stream_type& value()
	{
		if constexpr (std::is_same<stream_type, Source>::value || std::is_base_of<stream_type, Source>::value)
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
	static constexpr auto has_init(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::init(&std::declval<stream_type&>()), std::true_type{});
	static constexpr auto has_init(...) -> std::false_type;
	static constexpr decltype(has_init(std::declval<Traits&>())())	_init	= decltype(has_init(std::declval<Traits&>()))::value;



	template<class TestT>
	static constexpr auto has_close(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::close(std::declval<stream_type&>()), std::true_type{});
	static constexpr auto has_close(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_seek(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::seek(std::declval<stream_type&>(), std::declval<fpos_type&>(), streamif_base::beg), std::true_type{});
	static constexpr auto has_seek(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_tell(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::tell(std::declval<stream_type&>(), std::declval<fpos_type&>()), std::true_type{});
	static constexpr auto has_tell(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_size(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::size(std::declval<stream_type&>(), std::declval<size_type&>()), std::true_type{});
	static constexpr auto has_size(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_read(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::read(std::declval<stream_type&>(), std::declval<size_type&>(), std::declval<char*>(), std::declval<size_type&>()), std::true_type{});
	static constexpr auto has_read(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_write(TestT& v) -> decltype(std::declval<basic_streamif*>()->TestT::write(std::declval<stream_type&>(), std::declval<size_type&>(), std::declval<char const*>(), std::declval<size_type&>()), std::true_type{});
	static constexpr auto has_write(...) -> std::false_type;
	template<class TestT>
	static constexpr auto has_clear(TestT& v) -> decltype((decltype(std::declval<basic_streamif*>()->TestT::clear(std::declval<stream_type&>(), std::declval<iostate&>()))*)nullptr, std::true_type{});
	static constexpr auto has_clear(...) -> std::false_type;

	static constexpr decltype(has_close(std::declval<Traits&>())())	_close	= decltype(has_close(std::declval<Traits&>()))::value;
	static constexpr decltype(has_seek(std::declval<Traits&>())())	_seek	= decltype(has_seek(std::declval<Traits&>()))::value;
	static constexpr decltype(has_tell(std::declval<Traits&>())())	_tell	= decltype(has_tell(std::declval<Traits&>()))::value;
	static constexpr decltype(has_size(std::declval<Traits&>())())	_size	= decltype(has_size(std::declval<Traits&>()))::value;
	static constexpr decltype(has_read(std::declval<Traits&>())())	_read	= decltype(has_read(std::declval<Traits&>()))::value;
	static constexpr decltype(has_write(std::declval<Traits&>())())	_write	= decltype(has_write(std::declval<Traits&>()))::value;
	static constexpr decltype(has_clear(std::declval<Traits&>())())	_clear	= decltype(has_clear(std::declval<Traits&>()))::value;
};



template<class S>
struct	can_ostreamif
{
	using	type	= typename std::conditional<
							__iotest<S>::has_write_and_pcount
						||	__iotest<S>::has_write_return_size
						||	__iotest<S>::has_write_only
						,	std::true_type, std::false_type>::type;
};


template<class S>
using	can_ostreamif_t	= typename can_ostreamif<S>::type;


template<class S>
static constexpr can_ostreamif_t<S>	can_ostreamif_v	= can_ostreamif_t<S>{};

struct	streamif_traits_null{};


template<class Source, std::enable_if_t<can_ostreamif_v<Source>,int> = 0>
auto make_ostreamif(Source&& _value)
{
	return	basic_streamif<Source, streamif_traits_null>(std::move(_value));
}


template<class Source, std::enable_if_t<can_ostreamif_v<Source>,int> = 0>
auto make_ostreamif(Source* _value)
{
	return	basic_streamif<Source*, streamif_traits_null>(std::move(_value));
}



AMTRS_IO_NAMESPACE_END
#endif
