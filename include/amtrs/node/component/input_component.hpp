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
#ifndef	__libamtrs__node__component__input_component__hpp
#define	__libamtrs__node__component__input_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {

template<class NodeT>
class	input_component;

template<class NodeT>
class	input_dispatcher;


template<class NodeT>
class	basic_pointer_event
{
	using	_node_type	= NodeT;
public:
	basic_pointer_event() {}
	basic_pointer_event(input_event* _event) : mTop(_event) {}

	input_event* event() const noexcept { return mTop; }

protected:
	_node_type* capture() const noexcept { return mCapturing; }

	void set_capture(_node_type* _node)
	{
		mCapturing	= _node;
	}

	input_event*	mTop		= nullptr;
	_node_type*		mCapturing	= nullptr;

	template<class>
	friend	class	input_component;

	template<class>
	friend	class	input_dispatcher;
};


template<class NodeT>
class	input_component
{
public:
	using	pointer_event	= basic_pointer_event<NodeT>;

	//! イベントを処理した場合でも、キャプチャしない場合に、on_inputでこの値を返します。
	static constexpr NodeT* input_intercept() noexcept { return reinterpret_cast<NodeT*>(-1); }

protected:
	//! 継承した場合は派生先の処理を行う前に先に呼び出してください。
	//! イベントを処理したノードは自身のポインタを返してください。
	//! input_intercept を返すとイベントのキャプチャを行いません。
	virtual NodeT* on_input(pointer_event* _e)
	{
		NodeT*	ret	= _e->capture();
		if (!ret)
		{
			static_cast<NodeT*>(this)->visit_children_r([&](auto* c)
			{
				if (c->running_action_count() == 0 && c->visibility())		// アクション実行中は入力は無視する
				{
					if (auto r = c->on_input(_e); r)
					{
						ret	= r;
						return	true;
					}
				}
				return	false;
			});
		}
		return	ret;
	}

	//! 入力イベントがシステムによってキャンセルされたときに呼ばれます。
	virtual void on_input_cancel()
	{}


private:
	void input_cancel(NodeT* _capture)
	{
		if (_capture != static_cast<NodeT*>(this))
		{
			on_input_cancel();
		}
		static_cast<NodeT*>(this)->visit_children_r([&](auto* c)
		{
			c->input_cancel(_capture);
			return	0;
		});
	}

	template<class>
	friend	class	input_dispatcher;
};


template<class NodeT>
class	input_dispatcher
{
public:
	static constexpr NodeT* input_intercept() noexcept { return input_component<NodeT>::input_intercept(); }


	// 入力
	NodeT* input(input_component<NodeT>* _node, input_event* _event)
	{
		NodeT*		cap	= nullptr;
		basic_pointer_event<NodeT>	event(_event);
		auto						ret	= _node->on_input(&event);
		if (ret)
		{
			cap	= ret;
		}
		if (mLastCapture != cap && cap && (input_intercept() != cap))
		{
			_event->state		= input_event::cancel;
			basic_pointer_event<NodeT>	event(_event);
			_node->input_cancel(cap);
		}
		mLastCapture	= cap;
		return	cap;
	}

private:
	NodeT*	mLastCapture	= nullptr;
};


} AMTRS_NAMESPACE_END
#endif
