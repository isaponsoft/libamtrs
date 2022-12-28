/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
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

	
	virtual io::vstreamif on_open(path_type _path, std::error_code& _ec) override
	{
		struct	vfsstreambuf
				: public io::vstreamif::vstreambase
		{
			using	seekdir		= io::streamif_base::seekdir;
			using	openmode	= std::ios_base::openmode;
			using	off_type	= std::streambuf::off_type;
			using	fpos_type	= std::streambuf::pos_type;
			using	char_type	= std::istream::char_type;
			using	int_type	= std::istream::int_type;
			using	size_type	= std::streamsize;

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

			virtual void seek(fpos_type _pos, seekdir _dir) override
			{
				switch (_dir)
				{
					case seekdir::beg :
						mOffset	= _pos;
						break;

					case seekdir::cur :
						mOffset	+= _pos;
						break;

					case seekdir::end :
						mOffset	= mBufferSize + _pos;
						break;
				}
			}

			virtual fpos_type tell() override
			{
				return	mOffset;
			}

			virtual size_type size() override
			{
				return	mBufferSize;
			}

			virtual io::iostate rdstate() const noexcept override
			{
				return	mIOState;
			}

			virtual void clear(io::iostate _state) noexcept override
			{
			}

			virtual void read(void* _data, size_type _size) override
			{
				auto	readSize	= std::min<size_type>(_size, mBufferSize - mOffset);
				if (readSize)
				{
					std::memcpy(reinterpret_cast<char*>(_data), mBuffer + mOffset, readSize);
					mOffset += readSize;
				}
				mGCount	= readSize;
			}

			virtual void write(void const* _data, size_type _size) override
			{
			}

			virtual size_type gcount() const noexcept override
			{
				return	mGCount;
			}

			virtual size_type pcount() const noexcept override
			{
				return	0;
			}

			io::iostate	mIOState;
			off_type	mOffset	= 0;
			fpos_type	mGCount	= 0;
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

		return	io::vstreamif(new vfsstreambuf(asset));
	}

	AAssetManager*			mAsset;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
