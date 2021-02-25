/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__os__android__amtrs_activity__hpp
#define	__libamtrs__os__android__amtrs_activity__hpp

#include <jni.h>
#include <list>
#include <tuple>
#include "android-api-android_app.hpp"
#include "android-api-cpu.hpp"
#include "../application.hpp"
#include "../java/jp/libamtrs/AmtrsActivity.hpp"
#include "../java/android/content/Intent.hpp"

struct	amtrs_application_class;
jlong _AMTRS_GUI_MAIN(amtrs_application_class* _activity);


AMTRS_OS_ANDROID_NAMESPACE_BEGIN

class	amtrs_activity
		: public ref_object
		, public event_listener<void*, void(lifecycle)>
{
public:
	using	String			= java::classes::java::lang::String;
	using	Context			= java::classes::android::content::Context;
	using	Intent			= java::classes::android::content::Intent;
	using	AmtrsActivity	= java::classes::jp::libamtrs::AmtrsActivity;

	enum class	activirty_result
	{
		canceled	= 0,
		first_user	= 1,
		ok			= -1,
	};

	static void set_application(application* _app);
	virtual ~amtrs_activity();


	// ========================================================================
	//! メインアクティビティを取得します。
	// ------------------------------------------------------------------------
	static amtrs_activity*& get_instance();
	void regiseter_lifecycle_listener(void* _key, std::function<void(lifecycle)>&& _listener);
	void unregiseter_lifecycle_listener(void* _key);

	void regiseter_looper_listener(void* _key, std::function<void()>&& _listener);
	void unregiseter_looper_listener(void* _key);

	// main-android から呼ばれる。
	static jlong initialize(jobject _activity);


	//! RequestPermissionに対するコールバックを予約します。
	//! ActivityCompa.request... の requestCode にはこの関数の戻り値をセットしてください。
	//! コールバックを呼び出すと予約を削除します。
	int register_permissions_result(std::function<void(java::jobj<String[]>, java::jobj<jint[]>)> _callback);


	//! startActivity
	void startActivity(java::jobj<Intent> _intent, std::function<void(activirty_result, java::jobj<Intent>)> _callback);

private:
	amtrs_activity(jobject _activity, bool _mainActivity = false);


	void onCreate();
	void onStart();
	void onRestart();
	void onResume();
	void onPause();
	void onStop();
	void onDestroy();
	void onLooper();
	void onRequestPermissionsResult(jint _requestCode, java::jobj<String[]> _permissions, java::jobj<jint[]> _grantResults);
	void onActivityResult(jint _requestCode, jint _resultCode, java::jobj<Intent> _data);


	void fire_lc_event(lifecycle _lctype, application::system_event _event);

	static ref<amtrs_activity>& keep_instance();

	static inline ref<application>							sApplication;

	java::globalref<jobject>								mThiz;
	bool													mMainActivity			= false;
	event_listener<void*, void(lifecycle)>					mLifecycleListener;
	event_listener<void*, void()>							mLooperHandler;

	struct	permissions_result_info
	{
		int																requestCode;
		std::function<void(java::jobj<String[]>, java::jobj<jint[]>)>	callback;
	};
	std::list<permissions_result_info>						mPermissionsReultList;


	struct	activity_result
	{
		int															request_code;
		std::function<void(activirty_result, java::jobj<Intent>)>	callback;
	};
	std::list<activity_result>								mActivityResultList;
};



AMTRS_OS_ANDROID_NAMESPACE_END
#endif
