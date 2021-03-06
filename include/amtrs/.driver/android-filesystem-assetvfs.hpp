﻿/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef __libamtrs__filesystem__assetvfs_android__hpp
#define __libamtrs__filesystem__assetvfs_android__hpp
// ****************************************************************************
// Google Android のアセットに対して利用できる vfs。
// 必要に応じてメインプログラムから個別に呼び出してください。
// ****************************************************************************
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include "android-api-android_app.hpp"
#include "../java/android/content/res/AssetManager.hpp"
#include "../java/android/content/res/Resources.hpp"
AMTRS_FILESYSTEM_NAMESPACE_BEGIN


// ============================================================================
//! 標準ファイルシステムに対するインターフェース
// ----------------------------------------------------------------------------
//! std::filesystem を仮想化します。
// ----------------------------------------------------------------------------
class	assetvfs : public vfs
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

	assetvfs()
	{
		JNIEnv*			env	= java::get_jnienv();
		auto			am	= os::android::get_activity().getResources().getAssets();
		mAsset	= AAssetManager_fromJava(env, am.get());
	}

	virtual file_status    on_status(path_type _path, std::error_code& _ec) const override
	{
		auto*	asset	= AAssetManager_open(mAsset, ((std::string)_path).c_str(), AASSET_MODE_UNKNOWN);
		if (!asset)
		{
			AMTRS_SYSTRACE_LOG("No such asset %s", ((std::string)_path).c_str());
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			return	file_status(file_type::not_found, perms::unknown);
		}
		AAsset_close(asset);
		return	file_status(file_type::regular, perms::unknown);
	}


	virtual std::uintmax_t on_file_size(path_type _path, std::error_code& _ec) const override
	{
		auto*	asset	= AAssetManager_open(mAsset, ((std::string)_path).c_str(), AASSET_MODE_UNKNOWN);
		if (!asset)
		{
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			return	0;
		}
		auto	len		= AAsset_getRemainingLength(asset);
		AAsset_close(asset);
		return	len;
	}


	virtual bool on_remove(path_type _path, std::error_code& _ec) const override
	{
		return	false;
	}


	virtual std::uintmax_t on_remove_all(path_type _path, std::error_code& _ec) const override
	{
		return	false;
	}


	
	virtual iovstream on_open(path_type _path, std::error_code& _ec) override
	{
		struct	vfsstreambuf
				: public iovstream::vstreambuf::streamif
		{
			using	seekdir		= std::ios_base::seekdir;
			using	openmode	= std::ios_base::openmode;
			using	off_type	= std::streambuf::off_type;
			using	pos_type	= std::streambuf::pos_type;
			using	char_type	= std::istream::char_type;
			using	int_type	= std::istream::int_type;

			vfsstreambuf(AAsset* _asset)
				: mAsset(_asset)
			{
				mBuffer		= static_cast<const char*>(AAsset_getBuffer(mAsset));
				mBufferSize	= AAsset_getRemainingLength(mAsset);
			}

			~vfsstreambuf()
			{
				AAsset_close(mAsset);
			}

			virtual pos_type seekoff(off_type _off, seekdir _dir, openmode _which) override
			{
				switch (_dir)
				{
					case seekdir::beg :
						mOffset	= _off;
						break;

					case seekdir::cur :
						mOffset	+= _off;
						break;

					case seekdir::end :
						mOffset	= mBufferSize - _off;
						break;
				}
				return	mOffset;
			}

			virtual int_type underflow() override
			{
				return	mOffset < mBufferSize ? mBuffer[mOffset] : 0;
			}

			virtual int_type uflow() override
			{
				return	mOffset < mBufferSize ? mBuffer[mOffset++] : 0;
			}

			virtual std::streamsize xsgetn(char_type* _s, std::streamsize _count) override
			{
				std::size_t	readSize	= std::min<std::size_t>(_count, mBufferSize - mOffset);
				if (readSize)
				{
					std::memcpy(_s, mBuffer + mOffset, readSize);
					mOffset += readSize;
				}
				return	readSize;
			}

			off_type	mOffset	= 0;
			AAsset*		mAsset;
			const char*	mBuffer;
			std::size_t	mBufferSize;
		};

		auto*	asset	= AAssetManager_open(mAsset, ((std::string)_path).c_str(), AASSET_MODE_UNKNOWN);
		if (!asset)
		{
			_ec	= std::make_error_code(std::errc::no_such_file_or_directory);
			return	{};
		}

		return	iovstream(iovstream::vstreambuf(new vfsstreambuf(asset)));
	}

	AAssetManager*			mAsset;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
