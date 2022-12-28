/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__reader__hpp
#define	__libamtrs__io__reader__hpp
#include <iosfwd>
#include "../memory/endian.hpp"
#include "../memory/ref_object.hpp"
#include "../string/text.hpp"
#include "../typeutil/eval_test.hpp"
#include "fwd.hpp"
AMTRS_IO_NAMESPACE_BEGIN

// stream reader interface
class	reader : public reader_base
{
public:
	virtual size_type read(pointer _buff, size_type _len) = 0;
	virtual void seekg(std::streamoff _off, std::ios_base::seekdir _dir) = 0;

	template<class T>
	size_type read(T& v)
	{
		return	read(&v, sizeof(v));
	}

	template<class T>
	size_type read_be(T& v)
	{
		auto	s	= read(&v, sizeof(T));
		if (s == sizeof(T))
		{
			v	= endian_util::encode<endian::big>(v);
		}
		return	s;
	}

	template<class T>
	size_type read_le(T& v)
	{
		auto	s	= read(&v, sizeof(T));
		if (s == sizeof(T))
		{
			v	= endian_util::encode<endian::little>(v);
		}
		return	s;
	}
};


template<class R, class Traits>
class	stream_reader : public reader
{
public:
	using	traits_type	= Traits;
	using	reader_type	= R;

	stream_reader(reader_type _stream)
		: stream_reader(std::move(_stream), (traits_type*)nullptr)
	{}

	virtual size_type read(pointer _buff, size_type _len) override
	{
		return	mTraits.read(mReader, _buff, _len);
	}

	virtual void seekg(std::streamoff _off, std::ios_base::seekdir _dir) override
	{
		if constexpr (eval_test<traits_type, has_seek>::value)
		{
			mTraits.seekg(mReader, _off, _dir);
		}
	}

	auto reader() -> reader_type& { return mReader; }


protected:
	struct	has_cnst { template<class T> auto test(T& o) -> decltype(new T(std::declval<reader_type&>())); };
	struct	has_seek { template<class T> auto test(T& o) -> decltype(o.seekg(std::declval<reader_type&>(), std::declval<std::streamoff>(), std::declval<std::ios_base::seekdir>())); };

	template<class T>
	stream_reader(reader_type _stream, T*, std::enable_if_t<eval_test<T, has_cnst>::value>* _ = nullptr)
		: mReader(std::move(_stream))
		, mTraits(mReader)
	{}

	template<class T>
	stream_reader(reader_type _stream, T*, std::enable_if_t<!eval_test<T, has_cnst>::value>* _ = nullptr)
		: mReader(std::move(_stream))
	{}


	reader_type		mReader;
	traits_type		mTraits;
};



template<class CharT, class Traits>
struct	reader_traits<std::basic_istream<CharT, Traits>>
{
	using	size_type	= reader_base::size_type;
	using	pointer		= reader_base::pointer;
	using	reader_type	= std::basic_istream<CharT, Traits>;
	size_type read(reader_type& _stream, pointer _buff, size_type _len)
	{
		return	_stream.read(reinterpret_cast<CharT*>(_buff), _len).gcount();
	}
	void seekg(reader_type& _stream, std::streamoff _off, std::ios_base::seekdir _dir)
	{
		_stream.seekg(_off, _dir);
	}
};



template<class T, class Traits = reader_traits<T>>
auto make_reader(T _stream)
{
	return	new stream_reader<T, Traits>(std::move(_stream));
}

template<class CharT, class CharTraits, class Traits = reader_traits<std::basic_istream<CharT, CharTraits>&>>
auto make_reader(std::basic_istream<CharT, CharTraits>& _stream)
{
	return	new stream_reader<std::basic_istream<CharT, CharTraits>&, Traits>(_stream);
}

template<class CharT, class CharTraits, class Traits = reader_traits<std::basic_istream<CharT, CharTraits>*>>
auto make_reader(std::basic_istream<CharT, CharTraits>* _stream)
{
	return	new stream_reader<std::basic_istream<CharT, CharTraits>*, Traits>(_stream);
}


AMTRS_IO_NAMESPACE_END
#endif
