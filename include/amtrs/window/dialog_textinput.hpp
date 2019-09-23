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
