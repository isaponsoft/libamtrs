/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__writer__hpp
#define	__libamtrs__io__writer__hpp
#include <iosfwd>
#include "../memory/endian.hpp"
#include "../memory/ref_object.hpp"
#include "../string/text.hpp"
#include "../typeutil/eval_test.hpp"
#include "fwd.hpp"
AMTRS_IO_NAMESPACE_BEGIN

// stream write interface
class	writer : public writer_base
{
public:
	virtual size_type write(pointer _buff, size_type _len) = 0;
	virtual void seekp(std::streamoff _off, std::ios_base::seekdir _dir) = 0;

	template<class T>
	size_type write(T v)
	{
		return	write(&v, sizeof(v));
	}

	template<class T>
	size_type write_be(T v)
	{
		auto	t(endian_util::encode<endian::big>(v));
		return	write(&t, sizeof(t));
	}

	template<class T>
	size_type write_le(T v)
	{
		auto	t(endian_util::encode<endian::little>(v));
		return	write(&t, sizeof(t));
	}
};


template<class W, class Traits>
class	stream_writer : public writer
{
public:
	using	traits_type	= Traits;
	using	writer_type	= W;

	stream_writer(writer_type _stream)
		: stream_writer(std::move(_stream), (traits_type*)nullptr)
	{}


	virtual size_type write(pointer _buff, size_type _len) override
	{
		return	mTraits.write(mWriter, _buff, _len);
	}

	virtual void seekp(std::streamoff _off, std::ios_base::seekdir _dir) override
	{
		if constexpr (eval_test<traits_type, has_seek>::value)
		{
			mTraits.seekp(mWriter, _off, _dir);
		}
	}

	auto writer() -> typename std::add_lvalue_reference<writer_type>::type { return mWriter; }

protected:
	struct	has_cnst { template<class T> auto test(T& o) -> decltype(new T(std::declval<writer_type&>()                                                                        )); };
	struct	has_seek { template<class T> auto test(T& o) -> decltype(o.seekp(std::declval<writer_type&>(), std::declval<std::streamoff>(), std::declval<std::ios_base::seekdir>())); };

	template<class T>
	stream_writer(writer_type _stream, T*, std::enable_if_t<eval_test<T, has_cnst>::value>* _ = nullptr)
		: mWriter(std::move(_stream))
		, mTraits(mWriter)
	{}

	template<class T>
	stream_writer(writer_type _stream, T*, std::enable_if_t<!eval_test<T, has_cnst>::value>* _ = nullptr)
		: mWriter(std::move(_stream))
	{}


	writer_type		mWriter;
	traits_type		mTraits;
};



template<class CharT, class Traits>
struct	writer_traits<std::basic_ostream<CharT, Traits>>
{
	using	size_type	= writer_base::size_type;
	using	pointer		= writer_base::pointer;
	using	writer_type	= std::basic_ostream<CharT, Traits>;
	size_type write(writer_type& _writer, pointer _buff, size_type _len)
	{
		return	_writer.write(reinterpret_cast<typename std::add_const<CharT>::type*>(_buff), _len).good() ? _len : 0;
	}
	void seekp(writer_type& _stream, std::streamoff _off, std::ios_base::seekdir _dir)
	{
		_stream.seekp(_off, _dir);
	}
};



template<class T, class Traits = writer_traits<T>>
auto make_writer(T _stream)
{
	return	new stream_writer<T, Traits>(std::move(_stream));
}

template<class CharT, class CharTraits, class Traits = writer_traits<std::basic_ostream<CharT, CharTraits>&>>
auto make_writer(std::basic_ostream<CharT, CharTraits>& _stream)
{
	return	new stream_writer<std::basic_ostream<CharT, CharTraits>&, Traits>(_stream);
}

template<class CharT, class CharTraits, class Traits = writer_traits<std::basic_ostream<CharT, CharTraits>*>>
auto make_writer(std::basic_ostream<CharT, CharTraits>* _stream)
{
	return	new stream_writer<std::basic_ostream<CharT, CharTraits>*, Traits>(_stream);
}



AMTRS_IO_NAMESPACE_END
#endif
