/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__android__api__imagerender__hpp
#define	__libamtrs__driver__android__api__imagerender__hpp
#include <android/native_window_jni.h>
#include <media/NdkImage.h>
#include <media/NdkImageReader.h>
#include "android-api-def.hpp"
AMTRS_OS_ANDROID_NAMESPACE_BEGIN namespace imagerender {

class	imagerender
{
public:
	imagerender() = default;
	imagerender(imagerender const&) = delete;
	imagerender(AImageReader* v) : mRender(v) {}
	~imagerender() { clear(); }

	operator AImageReader* () const noexcept { return mRender; }

	void clear()
	{
		if (mRender)
		{
			AImageReader_delete(mRender);
			mRender	= nullptr;
		}
	}

	ANativeWindow* window() const noexcept { return mWindow; }

	auto create(size2<int> _size, AIMAGE_FORMATS _fmt, int _maxImages) -> media_status_t
	{
		clear();
		media_status_t	r	= AImageReader_new(_size.width, _size.height, _fmt, _maxImages, &mRender);
		if (r != AMEDIA_OK)
		{
			return	r;
		}
		if (auto r = AImageReader_getWindow(mRender, &mWindow); r != AMEDIA_OK)
		{
			return	r;
		}
		ANativeWindow_acquire(mWindow);
		return	AMEDIA_OK;
	}

	template<class Listener>
	auto set_listener(Listener* _listener) -> media_status_t
	{
		mListener	= _listener;

		AImageReader_ImageListener	callbacks	= {};
		callbacks.context			= this;
		callbacks.onImageAvailable	= [](void* context, AImageReader* reader)
		{
			auto	thiz	= reinterpret_cast<imagerender const*>(context);
			reinterpret_cast<Listener*>(thiz->mListener)->onImageAvailable(*thiz);
		};
		return	AImageReader_setImageListener(mRender, &callbacks);
	}

	auto acquire_next_image() const -> AImage*
	{
		AImage*		image	= nullptr;
		AImageReader_acquireNextImage(mRender, &image);
		return	image;
	}

private:
	void*			mListener	= nullptr;
	AImageReader*	mRender		= nullptr;
	ANativeWindow*	mWindow		= nullptr;
};




} AMTRS_OS_ANDROID_NAMESPACE_END
#endif
