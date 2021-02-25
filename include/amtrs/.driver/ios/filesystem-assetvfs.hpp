/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__assetvfs_ios__hpp
#define __libamtrs__filesystem__assetvfs_ios__hpp
// ****************************************************************************
// Google Android のアセットに対して利用できる vfs。
// 必要に応じてメインプログラムから個別に呼び出してください。
// ****************************************************************************
#import <Foundation/Foundation.h>
#include "../../filesystem/stdvfs.hpp"
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
		auto	p	= get_path(_path);
		if (::stat(p.c_str(), &st))
		{
			// ファイルが見つからなかった
			AMTRS_DEBUG_LOG("No such bundle file \"%s\" <= \"%s\"", std::string(_path).c_str(), p.c_str());
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			return	file_status(file_type::not_found, perms::unknown);
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
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
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
		auto	st	= status(_path);
		if (st.type() == file_type::not_found)
		{
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			return	{};
		}
		if (!is_regular_file(st))
		{
			ios::iovstream	retval;
			retval.setstate(std::ios::failbit);
			AMTRS_DEBUG_LOG("%s : asset virtual filesystem fail open.", ((std::string)_path).c_str());
			return	retval;
		}
		return	stdvfs::on_open(get_path(_path), _ec);
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
		return	mBundlePath +"/"+ std::string(_path);
	}

	mutable std::string	mBundlePath;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
