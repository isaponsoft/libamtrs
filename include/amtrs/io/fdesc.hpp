/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__io__fdesc__hpp
#define	__libamtrs__io__fdesc__hpp
#include "../string/text.hpp"
AMTRS_IO_NAMESPACE_BEGIN

struct stat
{
	using	time_type	= 	uint64_t;
	size_t		st_size		= -1;
	time_type	st_atime	= 0;
	time_type	st_mtime	= 0;
	time_type	st_ctime	= 0;

	bool empty() const noexcept { return st_size == 0 || st_size == -1; }
	bool bad() const noexcept { return st_size == -1; };
	bool good() const noexcept { return !bad(); }
};

/* File Descriptor
 * ::open, ::read, ::write, ::close を呼び出すラッパー
 * ファイルディスクリプタが通常 int なので、型を判別しやすくするのが目的。
 */
class	fdesc
{
public:
	enum open_mode
	{
		om_default	= 0,		// デフォルト動作
		om_always	= 1,		// 致命的なエラーが無い限りは成功するようにする
	};

	enum shared_mode
	{
		sm_read			= 1<<0,		// 共有オープンでREADを許可する
		sm_write		= 1<<1,		// 共有オープンでWRITEを許可する
		sm_readwrite	= sm_read|sm_write,
	};


	fdesc() = default;
	fdesc(fdesc const&) = default;
	fdesc(fdesc&& _r)
		: mDesc(_r.release())
	{}
	fdesc& operator = (fdesc const&) noexcept = default;
	fdesc& operator = (fdesc&& _r) noexcept { mDesc = _r.release(); return *this; }
	explicit fdesc(int _desc)
		: mDesc(_desc)
	{}
	~fdesc() {}

	/*
	 * 標準出力を取得します。
	 */
	AMTRS_API static fdesc stdo();

	/*
	 * 標準出力を取得します。
	 */
	AMTRS_API static fdesc stde();

	/*
	 * 標準出力を取得します。
	 */
	AMTRS_API static fdesc stdi();


	// default : ファイルが無い場合は作成し、存在する場合はエラーを返す。
	// always  : ファイルが無い場合は作成し、存在する場合は開く。
	AMTRS_API errorval create(char const* _filename, open_mode _mode, unsigned int _shared_mode = 0);

	// default : ファイルが無い場合はエラー、存在する場合は開く。
	// always  : ファイルが無い場合は作成し、存在する場合は開く。
	AMTRS_API errorval open(char const* _filename, open_mode _mode, unsigned int _shared_mode = 0);

	// ファイルが無い場合は作成し、存在する場合は開く。
	AMTRS_API errorval append(char const* _filename, unsigned int _shared_mode = 0);

	AMTRS_API stat status() const noexcept;

	int release() noexcept { int r(mDesc); mDesc = -1; return r; }
	int read(void* _buff, int _size);
	int write(void const* _buff, int _size);
	AMTRS_API void close();
	int get() const noexcept { return mDesc; }
	bool empty() const noexcept { return mDesc == -1; }

	AMTRS_API fdesc& operator << (text v);
	AMTRS_API fdesc& operator << (char v) { return write(v); }
	AMTRS_API fdesc& operator << (char const* v);
	AMTRS_API fdesc& operator << (unsigned int v) { return write(static_cast<uintmax_t>(v), 10); }
	AMTRS_API fdesc& operator << (int v) { return write(static_cast<intmax_t>(v), 10); }
	AMTRS_API fdesc& operator << (short v) { return write(static_cast<intmax_t>(v), 10); }
	AMTRS_API fdesc& operator << (intmax_t v) { return write(v, 10); }
	AMTRS_API fdesc& operator << (uintmax_t v) { return write(v, 10); }
	AMTRS_API fdesc& operator << (void*	v);
	AMTRS_API fdesc& write(intmax_t v, int base);
	AMTRS_API fdesc& write(uintmax_t v, int base);
	AMTRS_API fdesc& write_noconv(text v);
	AMTRS_API fdesc& write(char v);
	AMTRS_API fdesc& flush();

protected:
	int		mDesc	= -1;
};



AMTRS_IO_NAMESPACE_END
#endif
