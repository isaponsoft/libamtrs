/*
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__os__android__amtrs_activity__hpp
#define	__libamtrs__os__android__amtrs_activity__hpp

#include <list>
#include <tuple>
#include "android_app.hpp"
#include "cpu.hpp"
#include "../../application/@"
#include "../../java/jp/libamtrs/AmtrsActivity.hpp"

AMTRS_NAMESPACE_BEGIN
struct	activity_proxy;
AMTRS_NAMESPACE_END


AMTRS_OS_ANDROID_NAMESPACE_BEGIN




class	amtrs_activity
		: public ref_object
		, public event_listener<void*, void(lifecycle)>
{
public:
	using	Context			= java::classes::android::content::Context;
	using	AmtrsActivity	= java::classes::jp::libamtrs::AmtrsActivity;

	static void set_application(application* _app)
	{
		sApplication	= _app;
	}


	virtual ~amtrs_activity();


	// ========================================================================
	//! メインアクティビティを取得します。
	// ------------------------------------------------------------------------
	static ref<amtrs_activity>& get_instance();


	void regiseter_lifecycle_listener(void* _key, std::function<void(lifecycle)>&& _listener) { mLifecycleListener.register_listener(_key, std::move(_listener)); }
	void unregiseter_lifecycle_listener(void* _key) { mLifecycleListener.unregister_listener(_key); }

	void regiseter_looper_listener(void* _key, std::function<void()>&& _listener) { mLooperHandler.register_listener(_key, std::move(_listener)); }
	void unregiseter_looper_listener(void* _key) { mLooperHandler.unregister_listener(_key); }

private:
	amtrs_activity(jobject _activity, bool _mainActivity = false);

	// main-android から呼ばれる。
	static jlong initialize(jobject _activity);



	void onCreate()
	{
		//AMTRS_DEBUG_LOG("[%p] onCreate()", this);
		mLifecycleListener.fire_event(lifecycle::create);
		if (application::get_application())
		{
			application::get_application()->on_created(0, nullptr);
		}
	}

	void onStart()
	{
		//AMTRS_DEBUG_LOG("[%p] onStart()", this);
		fire_lc_event(lifecycle::start, application::system_event::start);
	}

	void onRestart()
	{
		//AMTRS_DEBUG_LOG("[%p] onRestart()", this);
		fire_lc_event(lifecycle::restart, application::system_event::restart);
	}

	void onResume()
	{
		//AMTRS_DEBUG_LOG("[%p] onResume()", this);
		fire_lc_event(lifecycle::resume, application::system_event::resume);
	}

	void onPause()
	{
		//AMTRS_DEBUG_LOG("[%p] onPause()", this);
		fire_lc_event(lifecycle::pause, application::system_event::pause);
	}

	void onStop()
	{
		//AMTRS_DEBUG_LOG("[%p] onStop()", this);
		fire_lc_event(lifecycle::stop, application::system_event::stop);
	}

	void onDestroy()
	{
		//AMTRS_DEBUG_LOG("[%p] onDestroy()", this);
		fire_lc_event(lifecycle::destroy, application::system_event::destroyed);
		if (mMainActivity)
		{
			get_instance()	= nullptr;
		}
	}

	void onLooper()
	{
		mLooperHandler.fire_event();
		if (application::get_application())
		{
			application::get_application()->on_update();
		}
	}

	void fire_lc_event(lifecycle _lctype, application::system_event _event)
	{
		try
		{
			mLifecycleListener.fire_event(_lctype);
		}
		catch (std::exception& _e)
		{
			AMTRS_DEBUG_LOG("Lifecycle exception: %s", _e.what());
			throw	_e;
		}

		try
		{
			application::get_application()->on_event(_event);
		}
		catch (std::exception& _e)
		{
			AMTRS_DEBUG_LOG("application on event exception: %s", _e.what());
			throw	_e;
		}
	}

	static inline ref<application>							sApplication;

	java::globalref<jobject>								mThiz;
	bool													mMainActivity			= false;
	event_listener<void*, void(lifecycle)>					mLifecycleListener;
	event_listener<void*, void()>							mLooperHandler;

	friend	amtrs::activity_proxy;
};


inline amtrs_activity::amtrs_activity(jobject _activity, bool _mainActivity)
	: mThiz(java::globalref<jobject>(_activity))
	, mMainActivity(_mainActivity)
{
#ifdef	DEBUG
	auto	cpufam	= os::android::cpu_feature::get_cpu_family();
	auto	cpufea	= os::android::cpu_feature::get_cpu_feature();
	if (mMainActivity)
	{
		AMTRS_DEBUG_LOG("[%p] constractor - MainActivity : %s", this, cpufam.c_str()/*, cpufea.c_str()*/);
	}
	else
	{
		AMTRS_DEBUG_LOG("[%p] constractor", this);
	}
#endif
}


inline amtrs_activity::~amtrs_activity()
{
	sApplication	= nullptr;
	if (mMainActivity)
	{
		AMTRS_DEBUG_LOG("[%p] destractor - MainActivity", this);
		get_instance()	= nullptr;
	}
	else
	{
		AMTRS_DEBUG_LOG("[%p] destractor", this);
	}
}


inline ref<amtrs_activity>& amtrs_activity::get_instance()
{
	static ref<amtrs_activity>	s_instance;
	return	s_instance;
}



inline jlong amtrs_activity::initialize(jobject _activity)
{
	AMTRS_IMPLEMENTS(get_android_app)::app_instance::main_activity()	= java::globalref<jobject>(_activity);

	get_instance()	= new amtrs_activity(_activity, true);

	AMTRS_JAVA_JNIMETHOD_REGISTER(AmtrsActivity, amtrs_activity,
		AMTRS_JAVA_MAKE_JNIMETHOD(onCreate),
		AMTRS_JAVA_MAKE_JNIMETHOD(onStart),
		AMTRS_JAVA_MAKE_JNIMETHOD(onRestart),
		AMTRS_JAVA_MAKE_JNIMETHOD(onResume),
		AMTRS_JAVA_MAKE_JNIMETHOD(onPause),
		AMTRS_JAVA_MAKE_JNIMETHOD(onStop),
		AMTRS_JAVA_MAKE_JNIMETHOD(onDestroy),
		AMTRS_JAVA_MAKE_JNIMETHOD(onLooper)
	);

	return	reinterpret_cast<jlong>(get_instance().get());
}




AMTRS_OS_ANDROID_NAMESPACE_END
#endif
