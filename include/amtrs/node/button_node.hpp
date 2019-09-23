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
#ifndef	__libamtrs__node__button_node__hpp
#define	__libamtrs__node__button_node__hpp
#include "node.hpp"
#include "updatable_node.hpp"
#include "ui_utils.hpp"
AMTRS_NAMESPACE_BEGIN



template<class NodeT>
class	button_node
		: public NodeT
{
	using	_base_type	= NodeT;
public:
	using	node_type	= typename _base_type::node_type;

	using	_base_type::_base_type;

	void set_trigger_callback(std::function<void(vec3<float>)>&& _callback)
	{
		mTriggerCallback	= std::move(_callback);
	}

	void set_long_press_callback(std::function<bool(vec3<float>)>&& _callback)
	{
		mLongTriggerCallback	= std::move(_callback);
	}

	//! ボタンの反応サイズを拡大します。
	void set_trigger_size_expand(const size2<float>& _size)
	{
		mButton.expandSize	= _size;
	}

	bool is_button_pressing() const noexcept { return mPressing; }

	void cancel()
	{
		mButton.reset();
		mPressing		= false;
	}

protected:
	// ========================================================================
	//! ボタンの押下状態が変化したときに呼ばれる。
	// ------------------------------------------------------------------------
	//! 表示するグラフィックの変更のみを行うこと。
	// ------------------------------------------------------------------------
	virtual void on_change_press_state(bool _press)
	{}


	// ========================================================================
	//! ボタンのトリガーガが発生したときに呼ばれる。
	// ------------------------------------------------------------------------
	virtual void on_button_trigger(vec3<float> _pos)
	{
		if (mTriggerCallback)
		{
			mTriggerCallback(_pos);
		}
	}


	// ========================================================================
	//! ボタンを押した状態でドラッグしたときに呼ばれる。
	// ------------------------------------------------------------------------
	virtual void on_drag_trigger(vec3<float> _pos)
	{}
	

	// ========================================================================
	//! ボタンのロングトリガーは発生したときに呼ばれる。
	// ------------------------------------------------------------------------
	//! \return
	//!		true を返した場合は release 時のイベントを呼ばないようにします。
	// ------------------------------------------------------------------------
	virtual bool on_long_press_trigger(vec3<float> _pos)
	{
		bool	pressCancel	= false;
		if (mLongTriggerCallback)
		{
			pressCancel		= mLongTriggerCallback(_pos);
		}
		return	pressCancel;
	}

	virtual void on_input_cancel() override
	{
		_base_type::on_input_cancel();
		mButton.cancel(*this);
		cancel();
		if (mPressing)
		{
			on_change_press_state(mPressing = false);
		}
	}


	virtual typename _base_type::node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		if (auto r = _base_type::on_input(_e); r)
		{
			return	r;
		}

		node_type*	retval	= nullptr;
		if (auto mouse = _e->event()->get_mouse_event(); mouse)
		{
			auto		mat		= this->world_matrix().inverse();
			auto		p		= vec3<float>(mouse->template get_point<float>(), 0) * mat;
			auto		pos		= vec2<float>(p);
			if (mouse->state == input_event::long_pressing)
			{
				if (this->size().is_inner(pos) && on_long_press_trigger(p))
				{
					mButton.reset();
					return	this;
				}
				return	nullptr;
			}
			auto		ev		= mButton.input(*this, mouse, &pos);
			switch (ev)
			{
				case uiutils::button::event::press :
					on_change_press_state(mPressing = true);
					break;

				case uiutils::button::event::cancel :
					on_change_press_state(mPressing = false);
					break;

				case uiutils::button::event::fire :
					on_change_press_state(mPressing = false);
					on_button_trigger(vec3<float>(pos, 0));
					retval			= this;
					break;

				case uiutils::button::event::in :
					on_change_press_state(mPressing = true);
					break;

				case uiutils::button::event::out :
					on_change_press_state(mPressing = false);
					break;

				default:
					break;
			}
			if (mPressing && (mouse->state == input_event::drag))
			{
				on_drag_trigger(p);
			}
			if (mPressing && (mouse->state == input_event::release))
			{
				on_change_press_state(mPressing = false);
			}
		}
		return	retval;
	}

	uiutils::button						mButton;
	bool								mPressing			= false;
	std::function<void(vec3<float>)>	mTriggerCallback;
	std::function<bool(vec3<float>)>	mLongTriggerCallback;
};


AMTRS_NAMESPACE_END
#endif
