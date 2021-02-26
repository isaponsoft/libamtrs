/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#include <android/keycodes.h>
#include <amtrs/application.hpp>
#include <amtrs/jni.hpp>
#include <amtrs/window.hpp>
#include <amtrs/java/android/view/KeyEvent.hpp>
#include <amtrs/java/jp/libamtrs/window/DialogTextInput.hpp>
AMTRS_NAMESPACE_BEGIN


class	textinput_dialog_android
		: public textinput_dialog
{
	using	_base_type	= textinput_dialog;

public:
	using	_base_type::notify_text_change;
	using	_base_type::notify_return;

	using	CharSequence	= java::classes::java::lang::CharSequence;
	using	String			= java::classes::java::lang::String;
	using	KeyEvent		= java::classes::android::view::KeyEvent;
	using	DialogTextInput	= java::classes::jp::libamtrs::window::DialogTextInput;


protected:
	void initialize()
	{
	}

	virtual void on_show() override
	{
		// NativeMethodを登録
		AMTRS_JAVA_JNIMETHOD_REGISTER(DialogTextInput, textinput_dialog_android
			, AMTRS_JAVA_MAKE_JNIMETHOD(onTextChange)
			, AMTRS_JAVA_MAKE_JNIMETHOD(onEnter)
		);

		auto	objAct	= os::android::get_activity();
		auto	objDti	= DialogTextInput::find().init(objAct, static_cast<jlong>(reinterpret_cast<std::intptr_t>(this)));
		auto	text	= java::jobj<CharSequence>(java::to_jstring(_base_type::text()));
		objAct.pushView(java::jobj<java::classes::android::view::View>(objDti));
		objDti.setText(text);
		mDTI	= objDti;
	}

	void onTextChange(java::jobj<String> _text)
	{
		mText	= _text.to_string();
		notify_text_change();
	}

	void onEnter()
	{
		notify_return();
	}

	virtual void on_text_change(std::string_view _text) override
	{
		mText	= _text;
		java::jobj<DialogTextInput>	thiz	= java::jobj<DialogTextInput>(mDTI.get());
		auto	text	= java::to_jstring(_base_type::text());
		thiz.setText(java::jobj<CharSequence>(text));
	}


	virtual void on_close() override
	{
		java::jobj<DialogTextInput>	thiz	= java::jobj<DialogTextInput>(mDTI.get());
		thiz.cancel();
	}

	java::globalref<jobject>		mDTI;

	friend	ref<textinput_dialog> textinput_dialog::create();
};


ref<textinput_dialog> textinput_dialog::create()
{
	ref<textinput_dialog_android>	retval	= new textinput_dialog_android();
	retval->initialize();
	return	retval;
};


AMTRS_NAMESPACE_END
