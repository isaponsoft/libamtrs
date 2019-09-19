/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef __libamtrs__filesystem__stdvfs__hpp
#define __libamtrs__filesystem__stdvfs__hpp
#include "vfs.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


// ============================================================================
//! 標準ファイルシステムに対するインターフェース
// ----------------------------------------------------------------------------
//! std::filesystem を仮想化します。
// ----------------------------------------------------------------------------
class	stdvfs : public vfs
{
public:
	static ref<stdvfs> get_instance()
	{
		if (instance() == nullptr)
		{
			instance() = new stdvfs();
		}
		return	instance();
	}

	virtual ~stdvfs()
	{
		instance() = nullptr;
	}

private:

	static stdvfs*& instance()
	{
		static	stdvfs*	thiz	= nullptr;
		return	thiz;
	}

protected:
	stdvfs() {}

	virtual file_status    on_status    (const path& _path, std::error_code& _ec) const override { return stdfs::status    (_path, _ec); }
	virtual std::uintmax_t on_file_size (const path& _path, std::error_code& _ec) const override { return stdfs::file_size (_path, _ec); }
	virtual bool		   on_remove    (const path& _path, std::error_code& _ec) const override { return stdfs::remove    (_path, _ec); }
	virtual std::uintmax_t on_remove_all(const path& _path, std::error_code& _ec) const override { return stdfs::remove_all(_path, _ec); }


	
	virtual ios::iovstream on_open(const path& _path, std::error_code& _ec) override
	{
		struct	vfsstreambuf
				: public ios::iovstream::vstreambuf::streamif
		{
			using	seekdir		= std::ios_base::seekdir;
			using	openmode	= std::ios_base::openmode;
			using	off_type	= std::streambuf::off_type;
			using	pos_type	= std::streambuf::pos_type;
			using	char_type	= std::istream::char_type;
			using	int_type	= std::istream::int_type;

			vfsstreambuf(std::ifstream&& _in)
				: fs(std::move(_in))
			{}

			virtual pos_type seekoff(off_type _off, seekdir _dir, openmode _which) override
			{
				return	fs.rdbuf()->pubseekoff(_off, _dir, _which);
			}

			virtual int_type underflow() override
			{
				return	fs.rdbuf()->sgetc();
			}

			virtual int_type uflow() override
			{
				return	fs.rdbuf()->sbumpc();
			}

			virtual std::streamsize xsgetn(char_type* _s, std::streamsize _count) override
			{
				return	fs.rdbuf()->sgetn(_s, _count);
			}

			std::ifstream	fs;
		};

		ios::iovstream	retval;
		std::ifstream	fin(_path.string(), std::ios::binary);
		if (fin.is_open())
		{
			retval	= ios::iovstream(ios::iovstream::vstreambuf(new vfsstreambuf(std::move(fin))));
		}
		else
		{
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
		}
		return	retval;
	}
};





AMTRS_FILESYSTEM_NAMESPACE_END
#endif
