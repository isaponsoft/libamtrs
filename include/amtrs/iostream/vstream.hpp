/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__iostream__vstream__hpp
#define	__libamtrs__iostream__vstream__hpp
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
