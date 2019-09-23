/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__window__timeinput_dialog_android__hpp
#define	__libamtrs__window__timeinput_dialog_android__hpp
#include <functional>
#include "../java/android/support/v4/app/DialogFragment.hpp"
#include "../java/android/support/v4/app/FragmentManager.hpp"
#include "../java/jp/libamtrs/window/DialogTimePicker.hpp"
#include "def.hpp"
#include "window.hpp"
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



inline ref<timeinput_dialog> timeinput_dialog::create()
{
	ref<timeinput_dialog_android>	thiz	= new timeinput_dialog_android();
	thiz->initialize();
	return	thiz;
};

AMTRS_NAMESPACE_END
#endif
