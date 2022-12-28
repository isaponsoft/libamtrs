/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__filedescriptor__hpp
#define	__libamtrs__io__filedescriptor__hpp
AMTRS_IO_NAMESPACE_BEGIN


/*
 * FDに対する操作を統一します。
 * インライン展開されるのでオーバーヘッドはほぼありません。
 * --------------------------------------------------------------------------
 * file descriptor wrapper.
 * stdio.h open, close, read, write, seek, tell
 */
class	fdesc
{
public:
	using	value_type	= int;
	using	iostate		= std::ios_base::iostate;
	using	off_type	= uintmax_t;
	using	pos_type	= uintmax_t;
	using	size_type	= size_t;

	static constexpr value_type	nval	= -1;
	static constexpr iostate	goodbit	= std::ios_base::goodbit;
	static constexpr iostate	badbit	= std::ios_base::badbit;
	static constexpr iostate	failbit	= std::ios_base::failbit;
	static constexpr iostate	eofbit	= std::ios_base::eofbit;


	fdesc() = default;
	fdesc(fdesc const&) = delete;
	fdesc(fdesc&& _r)
		: mDesc(_r.mDesc)
		, mGCount(_r.mGCount)
		, mPCount(_r.mPCount)
	{
		_r.mDesc	= nval;
	}

	fdesc(char const* _name, openmode _mode, accessmode _access)
	{
		mDesc	= _open(_name, _mode, _access);
		if (mDesc == nval)
		{
			setstate(failbit);
		}
	}

	~fdesc() { close(); }

	fdesc& read(void* _buff, size_type _requestsize)
	{
		if (good())
		{
			_read(_buff, _requestsize);
		}
		return	*this;
	}

	fdesc& write(void const* _buff, size_type _requestsize)
	{
		if (good())
		{
			_write(_buff, _requestsize);
		}
		return	*this;
	}

	void seek(off_type _off) { seek(_off, seekdir_beg); }
	void seek(off_type _off, seekdir _dir)
	{
		if (good())
		{
			_seek(_off, _dir);
		}
	}

	off_type tell()
	{
		if (good())
		{
			return	_tell();
		}
		return	0;
	}

	void close()
	{
		if (mDesc != nval)
		{
			_close();
		}
	}


	size_type gcount() const noexcept { return mGCount; }
	size_type pcount() const noexcept { return mPCount; }

	explicit operator bool() const { return good(); }
	bool good() const noexcept { return rdstate() == goodbit; }
	bool eof()  const noexcept { return rdstate() & eofbit  ? true : false; }
	bool fail() const noexcept { return rdstate() & failbit ? true : false; }
	bool bad()  const noexcept { return rdstate() & badbit  ? true : false; }
	iostate rdstate() const noexcept { return mStatus; }
	void setstate(iostate _state) noexcept { clear(rdstate()|_state); }
	void clear(iostate _state = goodbit) noexcept { mStatus = _state; }

protected:
	value_type _open(char const* _filename, openmode _mode, accessmode _access);
	fdesc& _read(void* _buff, size_type _requestsize);
	fdesc& _write(void const* _buff, size_type _requestsize);
	void _seek(off_type _off, seekdir _dir);
	off_type _tell();
	void _close();

	value_type	mDesc	= nval;
	size_type	mGCount	= 0;
	size_type	mPCount	= 0;
	iostate		mStatus	= goodbit;
};


AMTRS_IO_NAMESPACE_END
#include AMTRS_PLATFORM_INCLUDE(io-filedescriptor.hpp)
#endif
