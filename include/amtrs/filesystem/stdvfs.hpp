﻿/******************************************************************************
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
#ifndef __libamtrs__filesystem__stdvfs__hpp
#define __libamtrs__filesystem__stdvfs__hpp
#include <fstream>
#include <sys/stat.h>
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

	virtual file_status on_status(path_type _path, std::error_code& _ec) const override
	{
		struct stat	st;
		std::string	p(_path);
		if (::stat(p.c_str(), &st))
		{
			// ファイルが見つからなかった
			return	file_status(file_type::not_found);
		}

		// file type.
		file_status		retval;
		retval.type
		(
			S_ISREG (st.st_mode) ? file_type::regular   :
			S_ISDIR (st.st_mode) ? file_type::directory :
			S_ISLNK (st.st_mode) ? file_type::symlink   :
			S_ISBLK (st.st_mode) ? file_type::block     :
			S_ISCHR (st.st_mode) ? file_type::character :
			S_ISFIFO(st.st_mode) ? file_type::fifo      :
			S_ISSOCK(st.st_mode) ? file_type::socket    :
						   file_type::unknown
		);
		return	retval;
	}


	virtual std::uintmax_t on_file_size (path_type _path, std::error_code& _ec) const override
	{
		struct stat	st;
		std::string	p(_path);
		if (::stat(p.c_str(), &st))
		{
			return	0;
		}
		return	st.st_size;
	}


	virtual bool on_remove(path_type _path, std::error_code& _ec) const override
	{
		return false;
	}

	virtual std::uintmax_t on_remove_all(path_type _path, std::error_code& _ec) const override
	{
		return 0;
	}

	
	virtual ios::iovstream on_open(path_type _path, std::error_code& _ec) override
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
		std::ifstream	fin((std::string)_path, std::ios::binary);
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
