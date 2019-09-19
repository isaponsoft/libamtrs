/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android_media_AudioManager__hpp
#define	__libamtrs__android__java_classes__android_media_AudioManager__hpp
AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::media {

// https://developer.android.com/reference/android/media/AudioManager#USE_DEFAULT_STREAM_TYPE
AMTRS_JAVA_DEFINE_CLASS(AudioManager, java::lang::Object)
{
	AMTRS_JAVA_CLASS_SIGNATURE("android/media/AudioManager");


	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_ALARM)
		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_DTMF)
		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_MUSIC)
		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_NOTIFICATION)
		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_RING)
		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_SYSTEM)
		AMTRS_JAVA_DEFINE_FIELD(jint, STREAM_VOICE_CALL)
		AMTRS_JAVA_DEFINE_FIELD(jint, USE_DEFAULT_STREAM_TYPE)
		
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
