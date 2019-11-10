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
#ifndef	__libamtrs__iostream__fstream__hpp
#define	__libamtrs__iostream__fstream__hpp
#include <cstdio>
#include <iosfwd>
#include "rstream.hpp"
#include "wstream.hpp"
AMTRS_IOSTREAM_NAMESPACE_BEGIN

// ****************************************************************************
//! 様々なオブジェクトに対して、std::istream に近いインターフェースを提供する。
// ----------------------------------------------------------------------------
//! あくまでもインターフェースのエミュレートのみを行い、内部的にはほぼ何もしない。
//! そのため、コンパイラの最適化によってオーバーヘッドはほぼ存在しないはずである。
//! 利用できるのは read(), gcount(), seekg(), tellg(), ステータスの取得系のみ。
// ----------------------------------------------------------------------------
template<>
class	rstream<FILE*>
{
public:
	using	iostate		= std::ios_base::iostate;
	using	off_type	= long;
	using	pos_type	= long;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	rstream(const rstream&) = default;
	rstream(rstream&&) = default;
	rstream& operator = (const rstream&) = default;
	rstream& operator = (rstream&&) = default;

	rstream(FILE* _file)
		: mFile(_file)
	{}

	rstream& read(char* _buff, std::size_t _n)
	{
		if (good())
		{
			mGCount	= fread(_buff, 1, _n, mFile);
			if (mGCount < _n)
			{
				setstate(eofbit);
			}
		}
		return	*this;
	}

	std::size_t gcount() const noexcept { return mGCount; }

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }


	pos_type tellg()
	{
		return	good()
				? std::ftell(mFile)
				: pos_type(-1);
	}

	rstream& seekg(off_type _off, std::ios_base::seekdir _dir)
	{
		if (good())
		{
			switch (_dir)
			{
				case std::ios_base::beg:
				{
					if (std::fseek(mFile, _off, SEEK_SET))
					{
						setstate(failbit);
					}
					break;
				}

				case std::ios_base::cur:
				{
					if (std::fseek(mFile, _off, SEEK_CUR))
					{
						setstate(failbit);
					}
					break;
				}

				case std::ios_base::end:
				{
					if (std::fseek(mFile, _off, SEEK_END))
					{
						setstate(failbit);
					}
					break;
				}

				default: break;
			}
		}
		return	*this;
	}

private:
	FILE*		mFile	= nullptr;
	iostate		mStatus	= goodbit;
	std::size_t	mGCount	= 0;
};


// ****************************************************************************
//! 様々なオブジェクトに対して、std::istream に近いインターフェースを提供する。
// ----------------------------------------------------------------------------
//! あくまでもインターフェースのエミュレートのみを行い、内部的にはほぼ何もしない。
//! そのため、コンパイラの最適化によってオーバーヘッドはほぼ存在しないはずである。
//! 利用できるのは read(), gcount(), seekg(), tellg(), ステータスの取得系のみ。
// ----------------------------------------------------------------------------
template<>
class	wstream<FILE*>
{
public:
	using	iostate		= std::ios_base::iostate;
	using	off_type	= long;
	using	pos_type	= long;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;

	wstream(const wstream&) = default;
	wstream(wstream&&) = default;
	wstream& operator = (const wstream&) = default;
	wstream& operator = (wstream&&) = default;

	wstream(FILE* _file)
		: mFile(_file)
	{}

	wstream& write(const char* _buff, std::size_t _n)
	{
		if (good())
		{
			auto	ws = fwrite(_buff, 1, _n, mFile);
			if (ws < _n)
			{
				setstate(failbit);
			}
		}
		return	*this;
	}

	explicit operator bool() const { return good(); }

	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }

	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }


	pos_type tellp()
	{
		return	good()
				? std::ftell(mFile)
				: pos_type(-1);
	}

	wstream& seekp(off_type _off, std::ios_base::seekdir _dir)
	{
		if (good())
		{
			switch (_dir)
			{
				case std::ios_base::beg:
				{
					if (std::fseek(mFile, _off, SEEK_SET))
					{
						setstate(failbit);
					}
					break;
				}

				case std::ios_base::cur:
				{
					if (std::fseek(mFile, _off, SEEK_CUR))
					{
						setstate(failbit);
					}
					break;
				}

				case std::ios_base::end:
				{
					if (std::fseek(mFile, _off, SEEK_END))
					{
						setstate(failbit);
					}
					break;
				}

				default: break;
			}
		}
		return	*this;
	}

private:
	FILE*		mFile	= nullptr;
	iostate		mStatus	= goodbit;
};

AMTRS_IOSTREAM_NAMESPACE_END
#endif
