/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__listener_stream__hpp
#define	__libamtrs__io__listener_stream__hpp
AMTRS_IO_NAMESPACE_BEGIN


template<class CharT>
struct	on_read
{
	char const*	data;
	size_t		size;
};

template<class CharT>
struct	on_write
{
	char const*	data;
	size_t		size;
};




template<class CharT, class UpStream>
class	listener_stream
{
public:
	using	stream_type		= UpStream;
	using	char_type		= CharT;
	using	off_type		= typename std::basic_iostream<CharT>::off_type;
	using	pos_type		= typename std::basic_iostream<CharT>::pos_type;
	using	iostate			= typename std::basic_iostream<CharT>::iostate;
	using	size_type		= size_t;
	using	listener_type	= listener<listener_stream<CharT, UpStream>>;

	listener_stream() = delete;
	listener_stream(listener_stream&&) = default;
	listener_stream& operator = (listener_stream&& _r) = default;
	~listener_stream() = default;

	listener_stream(listener_type* _listener, stream_type* _upstream)
		: mStream(_upstream)
		, mListener(_listener)
	{}


	size_t read(char_type* _buff, size_t _n)
	{
		auto	s	= mStream->read(_buff, _n);
		(*mListener)(on_read<CharT>{ _buff, (size_t)s });
		return	s;
	}

	size_t write(char_type const* _buff, size_t _n)
	{
		auto	s	= mStream->write(_buff, _n);
		(*mListener)(on_write<CharT>{ _buff, (size_t)s });
		return	s;
	}



	explicit operator bool() const { return good(); }

	bool good() const noexcept { return mStream->good(); }
	bool eof()  const noexcept { return mStream->eof();  }
	bool fail() const noexcept { return mStream->fail(); }
	bool bad()  const noexcept { return mStream->bad();  }

	iostate rdstate() const noexcept { return mStream->rdstate(); }
	void setstate(iostate _state) noexcept { mStream->setstate(_state); }
	void clear(iostate _state = std::ios::goodbit) noexcept { mStream->clear(_state); }


private:
	stream_type*	mStream		= nullptr;
	listener_type*	mListener	= nullptr;
};


AMTRS_IO_NAMESPACE_END


AMTRS_NAMESPACE_BEGIN
template<class CharT, class UpStream>
struct	listener<io::listener_stream<CharT, UpStream>>
		: listener<void(io::on_read<CharT>), void(io::on_write<CharT>)>
{
};
AMTRS_NAMESPACE_END
#endif
