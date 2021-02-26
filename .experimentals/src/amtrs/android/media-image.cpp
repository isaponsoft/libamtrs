/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/media/image.hpp>
#include <amtrs/java/android/content/ContentResolver.hpp>
#include <amtrs/java/android/content/Intent.hpp>
#include <amtrs/java/android/net/Uri.hpp>
#include <amtrs/java/android/graphics/Bitmap.hpp>
#include <amtrs/java/android/graphics/BitmapFactory.hpp>
#include <amtrs/java/android/os/ParcelFileDescriptor.hpp>
#include <amtrs/java/java/io/FileDescriptor.hpp>
#include <amtrs/.driver/android-api-amtrs_activity.hpp>
#include <amtrs/.driver/android-api-android_app.hpp>
#include <amtrs/.driver/android-graphics-bitmap.hpp>
AMTRS_MEDIA_NAMESPACE_BEGIN

using	Uri				= java::classes::android::net::Uri;
using	String			= java::classes::java::lang::String;
using	BitmapFactory	= java::classes::android::graphics::BitmapFactory;

class	image_
		: public image
{
public:
	virtual void pickup(std::function<void(std::string)> _listener) override
	{
		auto	clsIntent	= java::classes::android::content::Intent::find();

		auto	objIntent	= clsIntent.init(clsIntent.get_ACTION_OPEN_DOCUMENT());

		objIntent.addCategory(clsIntent.get_CATEGORY_OPENABLE());
		objIntent.setType(java::jobj<java::classes::java::lang::String>(java::to_jstring("image/*")));

		os::android::amtrs_activity::get_instance()->startActivity(objIntent, [=](auto code, auto data)
		{
			if (code == os::android::amtrs_activity::activirty_result::ok)
			{
				auto	uri	= data.getData();
				auto	f	= std::to_string((jstring)uri.toString().get());
				_listener(f);
			}
			else
			{
				_listener({});
			}
		});
	}


	// ==============================================================
	//! 選択されたイメージを読み込んでビットマップを返します。
	// --------------------------------------------------------------
	virtual bitmap<rgba<uint8_t>> read(std::string const& _imageIdentify) override
	{
		auto	uri		= Uri::find().parse(java::jobj<String>(java::to_jstring(_imageIdentify)));
		auto	mode	= java::jobj<java::classes::java::lang::String>(java::to_jstring("r"));
		auto	pfd		= os::android::get_activity().getContentResolver().openFileDescriptor(uri, mode);
		if (java::has_exception())
		{
			AMTRS_WARN_LOG("Exception");
			java::clear_exception();
			return	{};
		}
		auto	bmp		= BitmapFactory::find().decodeFileDescriptor(pfd.getFileDescriptor());
		pfd.close();
		if (!bmp.get())
		{
			AMTRS_WARN_LOG("BMP NULL");
			return	{};
		}
		return	create_bitmap_from_android_bitmap<rgba<uint8_t>, std::vector<rgba<uint8_t>>>(java::get_jnienv(), bmp.get());
	}
};


ref<image> image::create()
{
	return	new image_();
}

AMTRS_MEDIA_NAMESPACE_END
