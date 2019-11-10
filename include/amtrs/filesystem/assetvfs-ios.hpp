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
#ifndef __libamtrs__filesystem__assetvfs_ios__hpp
#define __libamtrs__filesystem__assetvfs_ios__hpp
// ****************************************************************************
// Google Android のアセットに対して利用できる vfs。
// 必要に応じてメインプログラムから個別に呼び出してください。
// ****************************************************************************
#import <Foundation/Foundation.h>
#include "def.hpp"
#include "types.hpp"
#include "stdvfs.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


// ============================================================================
//! 標準ファイルシステムに対するインターフェース
// ----------------------------------------------------------------------------
//! std::filesystem を仮想化します。
// ----------------------------------------------------------------------------
class	assetvfs : public stdvfs
{
public:
	static ref<assetvfs> get_instance()
	{
		if (instance() == nullptr)
		{
			instance() = new assetvfs();
		}
		return	instance();
	}

	virtual ~assetvfs()
	{
		instance() = nullptr;
	}

private:

	static assetvfs*& instance()
	{
		static	assetvfs*	thiz	= nullptr;
		return	thiz;
	}

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
		auto	p	= get_path(_path);
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
		auto	p	= get_path(_path);
		if (!is_regular_file((path_type)p))
		{
			ios::iovstream	retval;
			retval.setstate(std::ios::failbit);
			AMTRS_TRACE_LOG("%s : asset virtual filesystem fail open.", ((std::string)_path).c_str());
			return	retval;
		}
		return	stdvfs::on_open(p, _ec);
	}

protected:
	std::string get_path(path_type _path) const
	{
		if (mBundlePath.empty())
		{
			NSBundle*	bundle	= [NSBundle mainBundle];
			NSString*	dir		= [bundle resourcePath];
			mBundlePath	= std::string((const char*)[dir UTF8String]);
		}
		return	mBundlePath +"/"+ _path;
	}

	mutable std::string	mBundlePath;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
