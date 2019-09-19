/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__iostream__vstream__hpp
#define	__libamtrs__iostream__vstream__hpp
#include <iostream>
#include "def.hpp"
AMTRS_IOSTREAM_NAMESPACE_BEGIN


// ============================================================================
//! std::iostream を抽象化し、ムーブ可能にしたもの
// ----------------------------------------------------------------------------
class	iovstream
		: public std::iostream
{
public:
	// ============================================================================
	//! std::streambuf を抽象化し、ムーブ可能にしたもの
	// ----------------------------------------------------------------------------
	class	vstreambuf
			: public std::streambuf
	{
	public:
		using	iostate		= std::ios::iostate;
		using	seekdir		= std::ios_base::seekdir;
		using	openmode	= std::ios_base::openmode;
		using	off_type	= std::streambuf::off_type;
		using	pos_type	= std::streambuf::pos_type;
		using	char_type	= std::istream::char_type;
		using	int_type	= std::istream::int_type;
		using	traits_type	= std::istream::traits_type;

		struct	streamif
				: public ref_object
		{
			using	iostate		= std::ios::iostate;
			using	seekdir		= std::ios_base::seekdir;
			using	openmode	= std::ios_base::openmode;
			using	off_type	= std::streambuf::off_type;
			using	pos_type	= std::streambuf::pos_type;
			using	char_type	= std::istream::char_type;
			using	int_type	= std::istream::int_type;

			virtual pos_type seekoff(off_type _off, seekdir _dir, openmode _which) = 0;
			virtual int_type underflow() = 0;
			virtual int_type uflow() = 0;
			virtual std::streamsize xsgetn(char_type* _s, std::streamsize _count) = 0;
		};

		vstreambuf() = default;
		vstreambuf(const vstreambuf&) = default;
		vstreambuf(vstreambuf&&) = default;
		vstreambuf& operator = (const vstreambuf&) = default;
		vstreambuf& operator = (vstreambuf&&) = default;

		vstreambuf(streamif* _if)
			: mStream(_if)
		{}

	public:

		virtual pos_type seekoff(off_type _off, seekdir _dir, openmode _which) override { return mStream.empty() ? pos_type(-1) : mStream->seekoff(_off, _dir, _which); }
		virtual int_type underflow() override { return mStream.empty() ? traits_type::eof() : mStream->underflow(); }
		virtual int_type uflow() override { return mStream.empty() ? traits_type::eof() : mStream->uflow(); }
		virtual std::streamsize xsgetn(char_type* _s, std::streamsize _count) override { return mStream.empty() ? std::streamsize(-1) : mStream->xsgetn(_s, _count); }
	private:
		ref<streamif>	mStream;
	};


	iovstream()
		: std::iostream(&mStream)
	{}
	iovstream(const iovstream&) = delete;
	iovstream& operator = (const iovstream&) = delete;
	iovstream(iovstream&& _r)
		: std::iostream(&mStream)
		, mStream(std::move(_r.mStream))
	{
	}
	iovstream& operator = (iovstream&& _r)
	{
		mStream	= std::move(_r.mStream);
		rdbuf(&mStream);
		return	*this;
	}
	explicit iovstream(vstreambuf&& _buff)
		: std::iostream(&mStream)
		, mStream(std::move(_buff))
	{}

private:
	vstreambuf	mStream;
};




AMTRS_IOSTREAM_NAMESPACE_END
#endif
