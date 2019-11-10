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
#ifndef __libamtrs__filesystem__assetvfs_android__hpp
#define __libamtrs__filesystem__assetvfs_android__hpp
// ****************************************************************************
// Google Android のアセットに対して利用できる vfs。
// 必要に応じてメインプログラムから個別に呼び出してください。
// ****************************************************************************
#include "../os/android/android_app.hpp"
#include "../java/android/content/res/AssetManager.hpp"
#include "../java/android/content/res/Resources.hpp"
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include "def.hpp"
#include "types.hpp"
#include "vfs.hpp"
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

		return	ios::iovstream(ios::iovstream::vstreambuf(new vfsstreambuf(asset)));
	}

	AAssetManager*			mAsset;
};


AMTRS_FILESYSTEM_NAMESPACE_END
#endif
