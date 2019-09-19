/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__window__textinput_dialog__hpp
#define	__libamtrs__window__textinput_dialog__hpp
#include <functional>
#include "def.hpp"
#include "window.hpp"
AMTRS_NAMESPACE_BEGIN


class	textinput_dialog
		: public ref_object
{
public:
	static constexpr std::size_t	nolimit	= static_cast<std::size_t>(-1);

	static ref<textinput_dialog> create();

	const std::string& text() const noexcept { return mText; }

	void set_text(std::string_view _text)
	{
		if (mText != _text)
		{
			mText	= _text;
			on_text_change(_text);
		}
	}

	textinput_dialog* set_changed_callback(std::function<bool(std::string_view)>&& _callback)
	{
		mChangedCallback	= _callback;
		return	this;
	}

	textinput_dialog* set_return_callback(std::function<void()>&& _callback)
	{
		mReturnCallback	= _callback;
		return	this;
	}

	void close()
	{
		on_close();
	}

protected:
	//! テキストが変更されたことを継承元へ通達します。
	void notify_text_change()
	{
		//AMTRS_DEBUG_LOG("notify_text_change %s", mText.c_str());
		if (mChangedCallback)
		{
			mChangedCallback(mText);
		}
	}


	void notify_return()
	{
		if (mReturnCallback)
		{
			mReturnCallback();
		}
	}


	//! set_text() によってテキストが変更されたことを派生クラスへ通達します。
	virtual void on_text_change(std::string_view _text) = 0;

	virtual void on_close() = 0;

	std::string									mText;
	std::string									mHelp;
	std::size_t									mMaxLength		= nolimit;
	std::function<int32_t(std::string_view)>	mChangedCallback;
	std::function<void()>						mReturnCallback;
};


AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(dialog_textinput)
#endif
