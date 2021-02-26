/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <functional>
#include <amtrs/jni.hpp>
#include <amtrs/window.hpp>
#include <amtrs/java/androidx/fragment/app/DialogFragment.hpp>
#include <amtrs/java/androidx/fragment/app/FragmentManager.hpp>
#include <amtrs/java/jp/libamtrs/window/DialogTimePicker.hpp>
AMTRS_NAMESPACE_BEGIN

class	timeinput_dialog_android
		: public timeinput_dialog
{
	using	_base_type	= timeinput_dialog;

	using	String				= java::classes::java::lang::String;
	using	Context				= java::classes::android::content::Context;
	using	DialogTimePicker	= java::classes::jp::libamtrs::window::DialogTimePicker;

public:
	using	_base_type::notify_event;
	using	_base_type::notify_set_time;


	virtual void on_close() override
	{
	}


	virtual void on_set_time(value_type _hour, value_type _min) override
	{
		java::jobj<DialogTimePicker>	thiz	= java::jobj<DialogTimePicker>(mThis.get());
		thiz.updateTime((jint)_hour, (jint)_min);
	}

	void initialize()
	{
	}

	virtual void on_show() override
	{
		AMTRS_JAVA_JNIMETHOD_REGISTER(DialogTimePicker, timeinput_dialog_android, 
			AMTRS_JAVA_MAKE_JNIMETHOD(onTimeSet)
		);

		auto	objAct		= os::android::get_activity();
		auto	objDialog	= DialogTimePicker::find().create(static_cast<jlong>(reinterpret_cast<std::intptr_t>(this)));
		auto	title		= java::jobj<String>(java::to_jstring("time"));
		auto	fm			= objAct.getSupportFragmentManager();
		objDialog.show(fm, title);
		mThis	= objDialog;
	}



private:
	void onTimeSet(jint _hour, jint _minute)
	{
		_base_type::notify_set_time(_hour, _minute);
		notify_event(event_type::ok);
	}


	java::globalref<jobject>		mThis;

	friend	timeinput_dialog;
};



ref<timeinput_dialog> timeinput_dialog::create()
{
	ref<timeinput_dialog_android>	thiz	= new timeinput_dialog_android();
	thiz->initialize();
	return	thiz;
};

AMTRS_NAMESPACE_END
