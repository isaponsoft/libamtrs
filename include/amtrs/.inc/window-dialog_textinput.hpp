/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__window__textinput_dialog__hpp
#define	__libamtrs__window__textinput_dialog__hpp
AMTRS_NAMESPACE_BEGIN


class	textinput_dialog
		: public ref_object
{
public:
	static constexpr std::size_t	nolimit	= static_cast<std::size_t>(-1);

	static ref<textinput_dialog> create();

	void show() { on_show(); }

	const std::string& text() const noexcept { return mText; }

	void set_string(std::string_view _text)
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

	virtual void on_show() = 0;

	//! set_string() によってテキストが変更されたことを派生クラスへ通達します。
	virtual void on_text_change(std::string_view _text) = 0;

	virtual void on_close() = 0;

	std::string									mText;
	std::string									mHelp;
	std::size_t									mMaxLength		= nolimit;
	std::function<int32_t(std::string_view)>	mChangedCallback;
	std::function<void()>						mReturnCallback;
};


AMTRS_NAMESPACE_END
#endif
