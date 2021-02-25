/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__window__window__hpp
#define	__libamtrs__window__window__hpp
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

	using	size_type			= size2<int>;


	enum class event_type
	{
		activate,				//!< アクティブになった
		created,				//!< ウィンドウの作成が完了した
		destroyed,				//!< ウィンドウの破棄が完了した
		focus,					//!< フォーカスを失った
		input,					//!< 入力デバイスからのイベント
		sizechanged,			//!< ウィンドウサイズが変更された
		repaint,				//!< 再描画通知
	};

	union	wmunion
	{
		wmunion()
			: activate()
		{}

		bool			activate;
		bool			focus;
		input_event*	input;
		size_type		sizechanged;
		rect<int>		repaint;
	};

	//! ウィンドウのイベント通知を受け取るためのインターフェース
	struct	event_data
	{
		event_type	type;
		wmunion		wm;

		event_data(event_type _type)
			: type(_type)
		{}

		operator event_type () const noexcept
		{
			return	type;
		}
	};


	//! return
	//!		メッセージを自分で処理し、システムには無視させる場合は true。	
	//!		システムのデフォルトに任せる場合は false。
	using	handler_func	= bool(window&, event_data const&);


	virtual ~window() { if (*window_instance() == this) { *window_instance() = nullptr; } }


	static ref<window> create(size_type _size, std::function<handler_func> _proc);

	static ref<window> create(size_type _size) { return create(_size, {}); }

	//! 最初に生成されたインスタンスを返します。
	static window* get_first_instance() { return *window_instance(); }


	virtual context_type get_context() const noexcept = 0;

	virtual size_type size() const noexcept = 0;

	// ========================================================================
	//! 再描画を要求します。
	// ------------------------------------------------------------------------
	//! まだ準備ができていない場合は false を返します。
	//! _repeventにコールバックを渡した場合、set_event_callbackの代わりに
	//! コールバックを呼び出します。
	// ------------------------------------------------------------------------
	virtual bool repaint(std::function<void()> _repevent = {}) = 0;

	//! 存在しているウィンドウの数を返します。
	static size_t alives() noexcept;


	//! OSのウィンドウメッセージを処理します。
	static void message() { message([](auto&, auto&) { return false; }); }

	//! OSのウィンドウメッセージを処理します。
	static void message(std::function<handler_func> const& _event);

	//!	messageを連続で呼び出します。
	//!	messageが存在しない場合は _idle を呼び出します。
	//! _idle が false を返すか、すべてのウィンドウが閉じられると関数を抜けます。
	static void messageloop(std::function<handler_func> const& _event, std::function<bool()> const& _idle);


	static void messageloop(std::function<bool()> const& _idle) { messageloop([](auto&, auto&) { return false; }, std::move(_idle)); }


protected:
	window(std::function<handler_func> _handler)
		: mHandler(std::move(_handler))
	{
		if (*window_instance() == nullptr)
		{
			*window_instance() = this;
		}
	}


	static window** window_instance() noexcept
	{
		static window* s_instance = nullptr;
		return	&s_instance;
	}


	std::function<handler_func>	mHandler;
};







inline void window::messageloop(std::function<handler_func> const& _event, std::function<bool()> const& _idle)
{
	do
	{
		message(_event);
		_idle();
	} while (alives() > 0);
}




AMTRS_NAMESPACE_END
#endif
