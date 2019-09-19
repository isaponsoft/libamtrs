#ifndef	__libamtrs__node__back_key_node__hpp
#define	__libamtrs__node__back_key_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! Androidのバックキー、もしくはそれに相当するキーイベントを監視します。
// ----------------------------------------------------------------------------
//! バックキーが押されたときに、コールバックを呼び出します。コールバックが
//! true を返した場合、バックキーをアプリが処理したものとしてOSに通知します。
// ****************************************************************************
template<class NodeT>
class	back_key_node
		: public NodeT
{
	using	_base_type	= NodeT;
public:
	using	_base_type::_base_type;

	void set_back_callback(std::function<bool()>&& _onBack)
	{
		mOnBackCallback	= _onBack;
	}


protected:
	virtual bool on_back()
	{
		if (mOnBackCallback)
		{
			return	mOnBackCallback();
		}
		return	false;
	}


	virtual typename _base_type::node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		if (auto r = _base_type::on_input(_e); r)
		{
			return	r;
		}
		if (auto keyboard = _e->event()->get_keyboard_event(); keyboard)
		{
			if (keyboard->state == input_event::status::press && keyboard->key == key_code::back_key)
			{
				if (on_back())
				{
					return	this;
				}
			}
		}
		return	nullptr;
	}

private:
	std::function<bool()>	mOnBackCallback;
};


AMTRS_NAMESPACE_END
#endif
