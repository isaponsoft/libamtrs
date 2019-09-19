/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__window__textinput_dialog_android__hpp
#define	__libamtrs__window__textinput_dialog_android__hpp
#include <functional>
#include <android/keycodes.h>
#include "../../application.hpp"
#include "../../java.hpp"
#include "../../java/android/view/KeyEvent.hpp"
#include "../../java/jp/libamtrs/window/DialogTextInput.hpp"
#include "def.hpp"
#include "window.hpp"
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
		// NativeMethodを登録
		AMTRS_JAVA_JNIMETHOD_REGISTER(DialogTextInput, textinput_dialog_android, 
			AMTRS_JAVA_MAKE_JNIMETHOD(onKeyDown),
			AMTRS_JAVA_MAKE_JNIMETHOD(onAfterTextChanged)
		);

		auto	objAct	= os::android::get_activity();
		auto	objDti	= DialogTextInput::find().init(objAct, static_cast<jlong>(reinterpret_cast<std::intptr_t>(this)));
		auto	text	= java::jobj<CharSequence>(java::to_jstring(_base_type::text()));
		objDti.setText(text);
		objAct.pushView(java::jobj<java::classes::android::view::View>(objDti));
		mDTI	= objDti;
	}

	jboolean onKeyDown(int _keyCode, java::jobj<KeyEvent> _event)
	{
		if (_keyCode == AKEYCODE_ENTER)
		{
			notify_return();
			return	true;
		}
		return	false;
	}

	void onAfterTextChanged(java::jobj<String> _text)
	{
		mText	= _text.to_string();
		notify_text_change();
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
		thiz.close();
	}

	java::globalref<jobject>		mDTI;

	friend	ref<textinput_dialog> textinput_dialog::create();
};


inline ref<textinput_dialog> textinput_dialog::create()
{
	ref<textinput_dialog_android>	retval	= new textinput_dialog_android();
	retval->initialize();
	return	retval;
};


AMTRS_NAMESPACE_END
#endif
