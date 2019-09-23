/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__android__java_classes__android__content__BroadcastReceiver__hpp
#define	__libamtrs__android__java_classes__android__content__BroadcastReceiver__hpp
#include <amtrs/java/def.hpp>

AMTRS_JAVA_CLASSES_NAMESPACE_BEGIN

namespace android::os {
struct	Bundle;
struct	IBinder;
}

namespace android::content {

struct	Context;

AMTRS_JAVA_DEFINE_CLASS(BroadcastReceiver , java::lang::Object)
{
	using	Bundle				= android::os::Bundle;
	using	IBinder				= android::os::IBinder;
	using	String				= java::lang::String;
	using	Object				= java::lang::Object;

	AMTRS_JAVA_CLASS_SIGNATURE("android/content/BroadcastReceiver");


	AMTRS_JAVA_DEFINE_CLASS(PendingResult , java::lang::Object)
	{
		AMTRS_JAVA_CLASS_SIGNATURE("android/content/BroadcastReceiver$PendingResult");

		AMTRS_JAVA_DEFINE_STATIC_MEMBER
		{
			AMTRS_JAVA_STATICS_BASIC;
		};

		AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
		{
			AMTRS_JAVA_DYNAMICS_BASIC;

			AMTRS_JAVA_DEFINE_METHOD(abortBroadcast
				, void()
			)

			AMTRS_JAVA_DEFINE_METHOD(clearAbortBroadcast
				, void()
			)

			AMTRS_JAVA_DEFINE_METHOD(finish
				, void()
			)

			AMTRS_JAVA_DEFINE_METHOD(getAbortBroadcast
				, jboolean()
			)

			AMTRS_JAVA_DEFINE_METHOD(getResultCode
				, jint()
			)

			AMTRS_JAVA_DEFINE_METHOD(getResultData
				, String()
			)

			AMTRS_JAVA_DEFINE_METHOD(getResultExtras
				, Bundle(jboolean makeMap)
			)

			AMTRS_JAVA_DEFINE_METHOD(setResult
				, void(int code, String data, Bundle extras)
			)

			AMTRS_JAVA_DEFINE_METHOD(setResultCode
				, void(int code)
			)

			AMTRS_JAVA_DEFINE_METHOD(setResultData
				, void(String data)
			)

			AMTRS_JAVA_DEFINE_METHOD(setResultExtras
				, void(Bundle extras)
			)
		};
	};



	// クラスメソッドとクラスフィールド
	AMTRS_JAVA_DEFINE_STATIC_MEMBER
	{
		AMTRS_JAVA_STATICS_BASIC;

		AMTRS_JAVA_DEFINE_INIT(
			  BroadcastReceiver()
		)
	};


	// 動的メソッドと動的フィールド
	AMTRS_JAVA_DEFINE_DYNAMICS_MEMBER
	{
		AMTRS_JAVA_DYNAMICS_BASIC;



		AMTRS_JAVA_DEFINE_METHOD(abortBroadcast
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(clearAbortBroadcast
			, void()
		)

		AMTRS_JAVA_DEFINE_METHOD(getAbortBroadcast
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(getDebugUnregister
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(getResultCode
			, jint()
		)

		AMTRS_JAVA_DEFINE_METHOD(getResultData
			, String()
		)

		AMTRS_JAVA_DEFINE_METHOD(getResultExtras
			, Bundle(jboolean makeMap)
		)

		AMTRS_JAVA_DEFINE_METHOD(goAsync
			, PendingResult()
		)

		AMTRS_JAVA_DEFINE_METHOD(isInitialStickyBroadcast
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(isOrderedBroadcast
			, jboolean()
		)

		AMTRS_JAVA_DEFINE_METHOD(onReceive
			, void(Context context, Intent intent)
		)

		AMTRS_JAVA_DEFINE_METHOD(peekService
			, IBinder(Context context, Intent intent)
		)

		AMTRS_JAVA_DEFINE_METHOD(setDebugUnregister
			, void(jboolean debug)
		)

		AMTRS_JAVA_DEFINE_METHOD(setOrderedHint
			, void(jboolean isOrdered)
		)

		AMTRS_JAVA_DEFINE_METHOD(setResult
			, void(int code, String data, Bundle extras)
		)

		AMTRS_JAVA_DEFINE_METHOD(setResultCode
			, void(int code)
		)

		AMTRS_JAVA_DEFINE_METHOD(setResultData
			, void(String data)
		)

		AMTRS_JAVA_DEFINE_METHOD(setResultExtras
			, void(Bundle extras)
		)
	};
};


}
AMTRS_JAVA_CLASSES_NAMESPACE_END
#endif
