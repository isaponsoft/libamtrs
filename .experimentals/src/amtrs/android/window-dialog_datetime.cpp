/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <functional>
#include <amtrs/jni.hpp>
#include <amtrs/window.hpp>
#include <amtrs/java/androidx/fragment/app/DialogFragment.hpp>
#include <amtrs/java/androidx/fragment/app/FragmentManager.hpp>
#include <amtrs/java/jp/libamtrs/window/DialogDatePicker.hpp>
AMTRS_NAMESPACE_BEGIN

class	dateinput_dialog_android
		: public dateinput_dialog
{
	using	String				= java::classes::java::lang::String;
	using	Context				= java::classes::android::content::Context;
	using	DialogDatePicker	= java::classes::jp::libamtrs::window::DialogDatePicker;

public:
	using	super_type	= dateinput_dialog;
	using	super_type::notify_event;
	using	super_type::notify_set_date;


	virtual void on_close() override
	{
	}


	virtual void on_set_date(value_type _year, value_type _mon, value_type _day) override
	{
		java::jobj<DialogDatePicker>	thiz	= java::jobj<DialogDatePicker>(mThis.get());
		thiz.updateDate(_year, _mon-1, _day);
	}

	void initialize()
	{
	}

	virtual void on_show() override
	{
		AMTRS_JAVA_JNIMETHOD_REGISTER(DialogDatePicker, dateinput_dialog_android, 
			AMTRS_JAVA_MAKE_JNIMETHOD(onDateSet)
		);

		auto	objAct		= os::android::get_activity();
		auto	objDialog	= DialogDatePicker::find().create(static_cast<jlong>(reinterpret_cast<std::intptr_t>(this)));
		auto	title		= java::jobj<String>(java::to_jstring("date"));
		auto	fm			= objAct.getSupportFragmentManager();
		objDialog.show(fm, title);
		mThis	= objDialog;
	}



private:
	void onDateSet(value_type _year, value_type _mon, value_type _day)
	{
		super_type::notify_set_date(_year, _mon+1, _day);
		notify_event(event_type::ok);
	}


	java::globalref<jobject>		mThis;

	friend	dateinput_dialog;
};



ref<dateinput_dialog> dateinput_dialog::create()
{
	ref<dateinput_dialog_android>	thiz	= new dateinput_dialog_android();
	thiz->initialize();
	return	thiz;
};

AMTRS_NAMESPACE_END
