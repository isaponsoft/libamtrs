/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__android__java_classes__android__provider__MediaStore__hpp
#define	__libamtrs__android__java_classes__android__provider__MediaStore__hpp
#include "../../java/lang/String.hpp"
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN
namespace android::content {
struct	Context;
struct	ContentResolver;
}
namespace android::database {
struct	Cursor;
}
namespace android::net {
struct	Uri;
}
namespace android::provider {


AMTRS_JAVA_DEFINE_CLASS(MediaStore, java::lang::Object)
{
	using	Context	= android::content::Context;
	using	String	= java::lang::String;
	using	Uri		= android::net::Uri;

	AMTRS_JAVA_CLASS_SIGNATURE("android/provider/MediaStore");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_IMAGE_CAPTURE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_IMAGE_CAPTURE_SECURE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_REVIEW)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_REVIEW_SECURE)
		AMTRS_JAVA_DEFINE_FIELD(String, ACTION_VIDEO_CAPTURE)
		AMTRS_JAVA_DEFINE_FIELD(String, AUTHORITY)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_BRIGHTNESS)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_DURATION_LIMIT)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_FINISH_ON_COMPLETION)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_FULL_SCREEN)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MEDIA_ALBUM)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MEDIA_ARTIST)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MEDIA_FOCUS)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MEDIA_PLAYLIST)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MEDIA_RADIO_CHANNEL)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MEDIA_TITLE)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_OUTPUT)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_SCREEN_ORIENTATION)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_SIZE_LIMIT)
		AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_VIDEO_QUALITY)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_MEDIA_PLAY_FROM_SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_MEDIA_SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_MUSIC_PLAYER)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_STILL_IMAGE_CAMERA)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_STILL_IMAGE_CAMERA_SECURE)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_TEXT_OPEN_FROM_SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_VIDEO_CAMERA)
		AMTRS_JAVA_DEFINE_FIELD(String, INTENT_ACTION_VIDEO_PLAY_FROM_SEARCH)
		AMTRS_JAVA_DEFINE_FIELD(String, MEDIA_IGNORE_FILENAME)
		AMTRS_JAVA_DEFINE_FIELD(String, MEDIA_SCANNER_VOLUME)
		AMTRS_JAVA_DEFINE_FIELD(String, META_DATA_STILL_IMAGE_CAMERA_PREWARM_SERVICE)
		AMTRS_JAVA_DEFINE_FIELD(String, UNKNOWN_STRING)
		AMTRS_JAVA_DEFINE_FIELD(String, VOLUME_EXTERNAL)
		AMTRS_JAVA_DEFINE_FIELD(String, VOLUME_EXTERNAL_PRIMARY)
		AMTRS_JAVA_DEFINE_FIELD(String, VOLUME_INTERNAL)

		AMTRS_JAVA_DEFINE_FIELD(Uri, AUTHORITY_URI)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};



	AMTRS_JAVA_DEFINE_CLASS(Audio, java::lang::Object)
	{
		using	String	= java::lang::String;
		using	Uri		= android::net::Uri;

		AMTRS_JAVA_CLASS_SIGNATURE("android/provider/MediaStore$Audio");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};

		AMTRS_JAVA_DEFINE_CLASS(Media, java::lang::Object)
		{
			using	String	= java::lang::String;
			using	Uri		= android::net::Uri;

			AMTRS_JAVA_CLASS_SIGNATURE("android/provider/MediaStore$Images$Media");


			// クラスメソッドとクラスフィールド
			AMTRS_JAVA_DEFINE_STATIC_MEMBER
			{
				AMTRS_JAVA_STATICS_BASIC;

				AMTRS_JAVA_DEFINE_FIELD(String, CONTENT_TYPE)
				AMTRS_JAVA_DEFINE_FIELD(String, DEFAULT_SORT_ORDER)
				AMTRS_JAVA_DEFINE_FIELD(String, ENTRY_CONTENT_TYPE)
				AMTRS_JAVA_DEFINE_FIELD(String, EXTRA_MAX_BYTES)
				AMTRS_JAVA_DEFINE_FIELD(String, RECORD_SOUND_ACTION)
				AMTRS_JAVA_DEFINE_FIELD(Uri, EXTERNAL_CONTENT_URI)
				AMTRS_JAVA_DEFINE_FIELD(Uri, INTERNAL_CONTENT_URI)
			};


			// 動的メソッドと動的フィールド
			AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
			{
				AMTRS_JAVA_DYNAMICS_BASIC;
			};
		};
	};



	AMTRS_JAVA_DEFINE_CLASS(Images, java::lang::Object)
	{
		using	String	= java::lang::String;
		using	Uri		= android::net::Uri;

		AMTRS_JAVA_CLASS_SIGNATURE("android/provider/MediaStore$Images");


		// クラスメソッドとクラスフィールド
		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};


		// 動的メソッドと動的フィールド
		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;
		};

		AMTRS_JAVA_DEFINE_CLASS(Media, java::lang::Object)
		{
			using	String	= java::lang::String;
			using	Uri		= android::net::Uri;

			AMTRS_JAVA_CLASS_SIGNATURE("android/provider/MediaStore$Images$Media");


			// クラスメソッドとクラスフィールド
			AMTRS_JAVA_DEFINE_STATIC_MEMBER
			{
				AMTRS_JAVA_STATICS_BASIC;

				AMTRS_JAVA_DEFINE_FIELD(String, CONTENT_TYPE)
				AMTRS_JAVA_DEFINE_FIELD(String, DEFAULT_SORT_ORDER)
				AMTRS_JAVA_DEFINE_FIELD(Uri, EXTERNAL_CONTENT_URI)
				AMTRS_JAVA_DEFINE_FIELD(Uri, INTERNAL_CONTENT_URI)
			};


			// 動的メソッドと動的フィールド
			AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
			{
				AMTRS_JAVA_DYNAMICS_BASIC;
			};
		};
	};


};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
