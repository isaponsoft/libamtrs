/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
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

	input_event const* event() const noexcept { return mTop; }

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
	using	pointer_event		= basic_pointer_event<NodeT>;
	using	input_dispacher		= input_dispatcher<NodeT>;

	//! �C�x���g�����������ꍇ�ł��A�L���v�`�����Ȃ��ꍇ�ɁAon_input�ł��̒l��Ԃ��܂��B
	static constexpr NodeT* input_intercept() noexcept { return reinterpret_cast<NodeT*>(-1); }

	void set_input_enable(bool _enable) noexcept { mInputEnable = _enable; }

protected:
	//! �p�������ꍇ�͔h����̏������s���O�ɐ�ɌĂяo���Ă��������B
	//! �C�x���g�����������m�[�h�͎��g�̃|�C���^��Ԃ��Ă��������B
	//! input_intercept ��Ԃ��ƃC�x���g�̃L���v�`�����s���܂���B
	virtual NodeT* on_input(pointer_event* _e)
	{
		NodeT*	ret	= _e->capture();
		if (!ret)
		{
			static_cast<NodeT*>(this)->visit_children_r([&](auto* c)
			{
				if (static_cast<input_component<NodeT>*>(c)->on_input_enable())		// �A�N�V�������s���͓��͖͂�������
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

	//! ���̓C�x���g���V�X�e���ɂ���ăL�����Z�����ꂽ�Ƃ��ɌĂ΂�܂��B
	virtual void on_input_cancel()
	{}


	//! ���͂��󂯕t���邩�ǂ����𔻒肷�邽�߂�input_component����Ă΂��B
	virtual bool on_input_enable() const
	{
		auto	thiz	= static_cast<NodeT const*>(this);
		return	thiz->running_action_count() == 0 && thiz->visibility() && mInputEnable;
	}


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

	bool			mInputEnable	= true;

	template<class>
	friend	class	input_dispatcher;
};


template<class NodeT>
class	input_dispatcher
{
public:
	static constexpr NodeT* input_intercept() noexcept { return input_component<NodeT>::input_intercept(); }


	// ����
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
