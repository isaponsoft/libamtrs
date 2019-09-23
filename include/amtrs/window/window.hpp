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
#ifndef	__libamtrs__window__window__hpp
#define	__libamtrs__window__window__hpp
#include <functional>
#include "../input/@"
#include "def.hpp"
#include "display.hpp"
AMTRS_NAMESPACE_BEGIN


class	window
		: public ref_object
{
public:
#if	AMTRS_CURRENT_PLATFORM == AMTRS_PLATFORM_IOS
	using	context_type	= UIView*;
#else
	using	context_type	= void*;
#endif

	using	size_type			= size2<unsigned int>;
	using	on_input_type		= bool(input_event* _event);


	enum class event_type
	{
		activate_in,			//!< アクティブになった
		activate_out,			//!< アクティブから外れた
		focus_gain,				//!< フォーカスを得た
		focus_lost,				//!< フォーカスを失った
		size_changed,			//!< ウィンドウサイズが変更された
		repaint,				//!< 再描画通知
	};

	//! ウィンドウのイベント通知を受け取るためのインターフェース
	struct	event_data
	{
		event_type	type;

		event_data(event_type _type)
			: type(_type)
		{}
	};


	struct	activate_event;
	struct	focus_event;
	struct	size_event;
	struct	repaint_event;

	virtual ~window() { if (*window_instance() == this) { *window_instance() = nullptr; } }

	template<class Callback>
	void set_event_callback(Callback&& _callback) { m_eventCallback = std::forward<Callback>(_callback); }


	static ref<window> create(size_type _size);

	//! 最初に生成されたインスタンスを返します。
	static window* get_first_instance() { return *window_instance(); }


	virtual context_type get_context() const noexcept = 0;

	virtual size_type size() const noexcept = 0;

	// ========================================================================
	//! 再描画を要求します。
	// ------------------------------------------------------------------------
	//! まだ準備ができていない場合は false を返します。
	// ------------------------------------------------------------------------
	virtual bool repaint() = 0;

	//! ウィンドウが入力イベントを受け取った特に呼び出すコールバックを設定できます。
	//! false を返すとプラットフォーム側に入力イベントを破棄するように通知します。
	std::function<on_input_type>		onInputEvent;


	// 実験のため一時的に設置(g3d::deviceに実装予定)
//	void set_renderer(std::function<bool()>&& _renderer) { mRenderer = _renderer; }



protected:
	window();

	void fire_event(event_type _type, const event_data* _data)
	{
		if (m_eventCallback)
		{
			m_eventCallback(_type, _data);
		}
	}


	std::function<void(event_type, const event_data*)>	m_eventCallback;

	static window** window_instance() noexcept;
};

struct	window::activate_event : event_data
{
	activate_event()
		: event_data(event_type::activate_in)
	{}
};

struct	window::focus_event : event_data
{
	focus_event()
		: event_data(event_type::activate_in)
	{}
};

struct	window::size_event : event_data
{
	size_event(size_type _size)
		: event_data(event_type::size_changed)
		, size(_size)
	{}

	size_type	size;
};


struct	window::repaint_event : event_data
{
	repaint_event(rect<unsigned int> _rect)
		: event_data(event_type::repaint)
		, rect(_rect)
	{}

	rect<unsigned int>		rect;		//!< 更新された領域
};



inline window::window()
{
	if (*window_instance() == nullptr)
	{
		*window_instance() = this;
	}
}

inline window** window::window_instance() noexcept
{
	static window* s_instance = nullptr;
	return	&s_instance;
}




AMTRS_NAMESPACE_END
#include AMTRS_MAKE_INCLUDE_PLATFORM(window)
#endif
