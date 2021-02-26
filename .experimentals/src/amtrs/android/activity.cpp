/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <amtrs/.driver/android-api-amtrs_activity.hpp>

AMTRS_OS_ANDROID_NAMESPACE_BEGIN




amtrs_activity::amtrs_activity(jobject _activity, bool _mainActivity)
	: mThiz(java::globalref<jobject>(_activity))
	, mMainActivity(_mainActivity)
{
	get_instance()	= this;

#ifdef	DEBUG
	auto	cpufam	= os::android::cpu_feature::get_cpu_family();
	auto	cpufea	= os::android::cpu_feature::get_cpu_feature();
	if (mMainActivity)
	{
		AMTRS_SYSTRACE_LOG("[%p] constractor - MainActivity : %s", this, cpufam.c_str()/*, cpufea.c_str()*/);
	}
	else
	{
		AMTRS_SYSTRACE_LOG("[%p] constractor", this);
	}
#endif
}


amtrs_activity::~amtrs_activity()
{
	sApplication	= nullptr;
	if (mMainActivity)
	{
		AMTRS_SYSTRACE_LOG("[%p] destractor - MainActivity", this);
	}
	else
	{
		AMTRS_SYSTRACE_LOG("[%p] destractor", this);
	}
}


void amtrs_activity::set_application(application* _app)
{
	sApplication	= _app;
}


amtrs_activity*& amtrs_activity::get_instance()
{
	static amtrs_activity*	s_instance	= nullptr;
	return	s_instance;
}


void amtrs_activity::regiseter_lifecycle_listener(void* _key, std::function<void(lifecycle)>&& _listener)
{
	mLifecycleListener.register_listener(_key, std::move(_listener));
}


void amtrs_activity::unregiseter_lifecycle_listener(void* _key)
{
	mLifecycleListener.unregister_listener(_key);
}


void amtrs_activity::regiseter_looper_listener(void* _key, std::function<void()>&& _listener)
{
	mLooperHandler.register_listener(_key, std::move(_listener));
}


void amtrs_activity::unregiseter_looper_listener(void* _key)
{
	mLooperHandler.unregister_listener(_key);
}


void amtrs_activity::onCreate()
{
	AMTRS_SYSTRACE_LOG("[%p] onCreate()", this);
	mLifecycleListener.fire_event(lifecycle::create);
	if (application::get_application())
	{
		application::get_application()->on_created(0, nullptr);
	}
}

void amtrs_activity::onStart()
{
	AMTRS_SYSTRACE_LOG("[%p] onStart()", this);
	fire_lc_event(lifecycle::start, application::system_event::start);
}

void amtrs_activity::onRestart()
{
	AMTRS_SYSTRACE_LOG("[%p] onRestart()", this);
	fire_lc_event(lifecycle::restart, application::system_event::restart);
}

void amtrs_activity::onResume()
{
	AMTRS_SYSTRACE_LOG("[%p] onResume()", this);
	fire_lc_event(lifecycle::resume, application::system_event::resume);
}

void amtrs_activity::onPause()
{
	AMTRS_SYSTRACE_LOG("[%p] onPause()", this);
	fire_lc_event(lifecycle::pause, application::system_event::pause);
}

void amtrs_activity::onStop()
{
	AMTRS_SYSTRACE_LOG("[%p] onStop()", this);
	fire_lc_event(lifecycle::stop, application::system_event::stop);
}

void amtrs_activity::onDestroy()
{
	AMTRS_SYSTRACE_LOG("[%p] onDestroy()", this);
	fire_lc_event(lifecycle::destroy, application::system_event::destroyed);
	if (mMainActivity)
	{
		keep_instance()	= nullptr;
	}
}


void amtrs_activity::onRequestPermissionsResult(jint _requestCode, java::jobj<String[]> _permissions, java::jobj<jint[]> _grantResults)
{
	AMTRS_SYSTRACE_LOG("[%p] onRequestPermissionsResult(%d, %p, %p)", this, _requestCode, _permissions.get(), _grantResults.get());
	auto	m	= std::find_if(mPermissionsReultList.begin(), mPermissionsReultList.end(), [_requestCode](auto& l)
				{
					return	l.requestCode == _requestCode;
				});
	if (m == mPermissionsReultList.end())
	{
		AMTRS_WARN_LOG("Unkown requestCode %d", _requestCode);
		return;
	}
	m->callback(_permissions, _grantResults);
	mPermissionsReultList.erase(m);
}


void amtrs_activity::onLooper()
{
	mLooperHandler.fire_event();
	if (application::get_application())
	{
		application::get_application()->on_update();
	}
}

void amtrs_activity::fire_lc_event(lifecycle _lctype, application::system_event _event)
{
	try
	{
		mLifecycleListener.fire_event(_lctype);
	}
	catch (std::exception& _e)
	{
		AMTRS_WARN_LOG("Lifecycle exception: %s", _e.what());
		throw	_e;
	}

	try
	{
		application::get_application()->on_event(_event);
	}
	catch (std::exception& _e)
	{
		AMTRS_WARN_LOG("application on event exception: %s", _e.what());
		throw	_e;
	}
}

ref<amtrs_activity>& amtrs_activity::keep_instance()
{
	static ref<amtrs_activity>	s_instance;
	return	s_instance;
}



int amtrs_activity::register_permissions_result(std::function<void(java::jobj<String[]>, java::jobj<jint[]>)> _callback)
{
	int	rcode	= 1;
	for (;;)
	{
		auto	m	= std::find_if(mPermissionsReultList.begin(), mPermissionsReultList.end(), [rcode](auto& l)
					{
						return	l.requestCode == rcode;
					});
		if (m != mPermissionsReultList.end())
		{
			++rcode;
			continue;
		}
		break;
	}

	mPermissionsReultList.push_back(permissions_result_info{rcode, std::move(_callback)});
	return	rcode;
}


void amtrs_activity::startActivity(java::jobj<Intent> _intent, std::function<void(activirty_result, java::jobj<Intent>)> _callback)
{
	int	reqCode	= 1;
	for (;;)
	{
		bool	use	= false;
		for (auto& a : mActivityResultList)
		{
			if (a.request_code == reqCode)
			{
				use	= true;
				break;
			}
		}
		if (use)
		{
			++reqCode;
			continue;
		}
		break;
	}

	mActivityResultList.push_back({ reqCode, std::move(_callback) });
	os::android::get_activity().startActivityForResult(_intent, reqCode);
}


void amtrs_activity::onActivityResult(jint _requestCode, jint _resultCode, java::jobj<Intent> _data)
{
	AMTRS_SYSTRACE_LOG("[%p] onActivityResult(%d, %d, %p)", this, _requestCode, _resultCode, _data.get());
	for (auto it = mActivityResultList.begin(); it != mActivityResultList.end(); ++it)
	{
		if (it->request_code == _requestCode)
		{
			auto	callback	= std::move(it->callback);
			mActivityResultList.erase(it);
			callback((activirty_result)_resultCode, _data);
			return;
		}
	}
}


jlong amtrs_activity::initialize(jobject _activity)
{
	AMTRS_IMPLEMENTS(get_android_app)::app_instance::main_activity()	= java::globalref<jobject>(_activity);

	keep_instance()	= new amtrs_activity(_activity, true);

	AMTRS_JAVA_JNIMETHOD_REGISTER(AmtrsActivity, amtrs_activity,
		AMTRS_JAVA_MAKE_JNIMETHOD(onCreate),
		AMTRS_JAVA_MAKE_JNIMETHOD(onStart),
		AMTRS_JAVA_MAKE_JNIMETHOD(onRestart),
		AMTRS_JAVA_MAKE_JNIMETHOD(onResume),
		AMTRS_JAVA_MAKE_JNIMETHOD(onPause),
		AMTRS_JAVA_MAKE_JNIMETHOD(onStop),
		AMTRS_JAVA_MAKE_JNIMETHOD(onDestroy),
		AMTRS_JAVA_MAKE_JNIMETHOD(onLooper),
		AMTRS_JAVA_MAKE_JNIMETHOD(onRequestPermissionsResult),
		AMTRS_JAVA_MAKE_JNIMETHOD(onActivityResult)
	);

	return	reinterpret_cast<jlong>(get_instance());
}




AMTRS_OS_ANDROID_NAMESPACE_END
