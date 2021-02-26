/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__stream_in_stl__hpp
#define	__libamtrs__io__stream_in_stl__hpp
AMTRS_IO_NAMESPACE_BEGIN



template<class CharT, class Traits>
class	stream_in<std::basic_istream<CharT, Traits>>
{
public:
	using	_stream_type= std::basic_istream<CharT, Traits>;
	using	iostate			= std::ios_base::iostate;
	using	off_type		= typename _stream_type::off_type;
	using	pos_type		= typename _stream_type::pos_type;
	using	char_type		= typename _stream_type::char_type;
	using	size_type		= uintmax_t;

	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	stream_in(_stream_type* _stream)
		: mStream(*_stream)
	{}

	stream_in(const stream_in&) = default;
	stream_in(stream_in&&) = default;
	stream_in& operator = (const stream_in&) = default;
	stream_in& operator = (stream_in&&) = default;

	stream_in& read(char_type* _buff, std::size_t _n)
	{
		mStream.read(_buff, _n);
		return	*this;
	}

	std::size_t gcount() const noexcept { return (std::size_t)mStream.gcount(); }

	explicit operator bool() const { return (bool)mStream; }

	bool good() const noexcept { return mStream.good(); }
	bool eof()  const noexcept { return mStream.eof(); }
	bool fail() const noexcept { return mStream.fail(); }
	bool bad()  const noexcept { return mStream.bad(); }

	iostate rdstate() const noexcept { return mStream.rdstate(); }
	void setstate(iostate _state) noexcept { mStream.setstate(_state); }
	void clear(iostate _state = goodbit) noexcept { mStream.clear(_state); }


	pos_type tellg() { return mStream.tellg(); }
	stream_in& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		mStream.seekg(_off, _dir);
		return	*this;
	}

	size_type size()
	{
		clear();
		auto	curPos	= tellg();
		seekg(0, std::ios::end);
		auto	endPos	= tellg();
		clear();
		seekg(0, std::ios::beg);
		auto	begPos	= tellg();
		clear();
		seekg(curPos,  std::ios::beg);
		return	static_cast<uintmax_t>(endPos - begPos);
	}

protected:
	_stream_type&	mStream;
};



template<class CharT, class Traits>
class	stream_in<std::basic_fstream<CharT, Traits>>
		: public stream_in<std::basic_istream<CharT, Traits>>
{};


AMTRS_IO_NAMESPACE_END
#endif
