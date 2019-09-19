/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
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

	virtual file_status    on_status    (const path& _path, std::error_code& _ec) const override { return filesystem::status    (get_path(_path), _ec); }
	virtual std::uintmax_t on_file_size (const path& _path, std::error_code& _ec) const override { return filesystem::file_size (get_path(_path), _ec); }
	virtual bool		   on_remove    (const path& _path, std::error_code& _ec) const override { return filesystem::remove    (get_path(_path), _ec); }
	virtual std::uintmax_t on_remove_all(const path& _path, std::error_code& _ec) const override { return filesystem::remove_all(get_path(_path), _ec); }
	
	virtual ios::iovstream on_open(const path& _path, std::error_code& _ec) override
	{
		auto	p	= get_path(_path);
		if (!filesystem::is_regular_file(p))
		{
			ios::iovstream	retval;
			retval.setstate(std::ios::failbit);
			AMTRS_TRACE_LOG("%s : asset virtual filesystem fail open.", _path.string().c_str());
			return	retval;
		}
		return	stdvfs::on_open(p, _ec);
	}

protected:
	path get_path(const path& _path) const
	{
		if (mBundlePath.empty())
		{
			NSBundle*	bundle	= [NSBundle mainBundle];
			NSString*	dir		= [bundle resourcePath];
			mBundlePath	= path(std::string((const char*)[dir UTF8String]));
		}
		return	mBundlePath / _path;
	}

	mutable path	mBundlePath;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
