/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__button_node__hpp
#define	__libamtrs__node__button_node__hpp
#include "node.hpp"
#include "updatable_node.hpp"
#include "ui_utils.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(button_node)
template<class NodeT>
struct button_node_valid
{};


template<class SuperT>
class	button_node_impl
		: public SuperT
		, public button_node_valid<typename SuperT::node_type>
{
public:
	using	super_type	= SuperT;
	using	node_type	= typename super_type::node_type;

	using	super_type::super_type;

	void set_input_enable(bool _enable)
	{
		mInputEnable	= _enable;
	}

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
		super_type::on_input_cancel();
		mButton.cancel(*this);
		cancel();
		if (mPressing)
		{
			on_change_press_state(mPressing = false);
		}
	}


	virtual typename super_type::node_type* on_input(typename super_type::pointer_event* _e) override
	{
		if (auto r = super_type::on_input(_e); r)
		{
			return	r;
		}
		if (!mInputEnable)
		{
			return	nullptr;
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
	bool								mInputEnable		= true;
	bool								mPressing			= false;
	std::function<void(vec3<float>)>	mTriggerCallback;
	std::function<bool(vec3<float>)>	mLongTriggerCallback;
};
AMTRS_IMPLEMENTS_END(button_node)


// ****************************************************************************
//! ボタン機能を持たせるノード
// ----------------------------------------------------------------------------
//! アイテムとは子ノードの扱いの一つです。リストタイプのノードなどはアイテムを
//! 自動的に整理するなど、親ノードによる管理を行うことを前提とした子ノードと
//! なります。
// ----------------------------------------------------------------------------
template<class SuperT>
using	button_node	=
			typename std::conditional<
				std::is_base_of<AMTRS_IMPLEMENTS(button_node)::button_node_valid<typename SuperT::node_type>, SuperT>::value,
				SuperT,															
				AMTRS_IMPLEMENTS(button_node)::button_node_impl<SuperT>
			>::type;


AMTRS_NAMESPACE_END
#endif
