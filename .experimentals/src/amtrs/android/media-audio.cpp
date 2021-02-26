/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <thread>
#include <amtrs/media/audio.hpp>
#include <amtrs/java/android/content/Context.hpp>
#include <amtrs/java/android/content/ContentResolver.hpp>
#include <amtrs/java/android/content/Intent.hpp>
#include <amtrs/java/android/net/Uri.hpp>
#include <amtrs/java/android/media/MediaPlayer.hpp>
#include <amtrs/java/android/os/ParcelFileDescriptor.hpp>
#include <amtrs/java/java/io/FileDescriptor.hpp>
#include <amtrs/.driver/android-api-amtrs_activity.hpp>
#include <amtrs/.driver/android-api-android_app.hpp>
AMTRS_MEDIA_NAMESPACE_BEGIN

using	Uri				= java::classes::android::net::Uri;
using	String			= java::classes::java::lang::String;
using	BitmapFactory	= java::classes::android::graphics::BitmapFactory;
using	MediaPlayer		= java::classes::android::media::MediaPlayer;

static	jobject	mMediaPlayer	= nullptr;

class	audio_
		: public audio
{
public:
	virtual void pickup(std::function<void(std::string)> _listener) override
	{
		auto	clsIntent	= java::classes::android::content::Intent::find();

		auto	objIntent	= clsIntent.init(clsIntent.get_ACTION_OPEN_DOCUMENT());

		objIntent.addCategory(clsIntent.get_CATEGORY_OPENABLE());
		objIntent.setType(java::jobj<java::classes::java::lang::String>(java::to_jstring("audio/*")));

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


	virtual void play(std::string const& _identify) override
	{
		auto	uri		= Uri::find().parse(java::jobj<String>(java::to_jstring(_identify)));
		auto	mode	= java::jobj<java::classes::java::lang::String>(java::to_jstring("r"));
		auto	pfd		= os::android::get_activity().getContentResolver().openFileDescriptor(uri, mode);
		if (java::has_exception())
		{
			AMTRS_WARN_LOG("Audio open error");
			java::clear_exception();
			return;
		}
		if (mMediaPlayer)
		{
			java::get_jnienv()->DeleteGlobalRef(mMediaPlayer);
		}
		auto		mp	= MediaPlayer::find().create(os::android::get_activity(), uri);
		mp.start();
		mMediaPlayer	= java::get_jnienv()->NewGlobalRef(mp.get());
	}

};


ref<audio> audio::create()
{
	return	new audio_();
}

AMTRS_MEDIA_NAMESPACE_END
