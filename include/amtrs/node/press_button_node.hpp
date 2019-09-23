#ifndef	__libamtrs__node__press_button_node__hpp
#define	__libamtrs__node__press_button_node__hpp
#include "sprite_node.hpp"
AMTRS_NAMESPACE_BEGIN

//! 押しただけで反応するボタン
template<class NodeT>
class	press_button_node
		: public NodeT
{
	using	_base_type	= NodeT;
public:

	std::function<void()>	onButtonTrigger;

	template<class... Args>
	press_button_node(Args&&... _args)
		: _base_type(std::forward<Args>(_args)...)
	{}

	// ========================================================================
	//! キーボードを押したときにも反応するようにします。
	// ------------------------------------------------------------------------
	void set_key_code(key_code _kc)
	{
		mKeyCode = _kc;
	}

protected:
	// ========================================================================
	//! ボタンノトリガーガが発生したときに呼ばれる。
	// ------------------------------------------------------------------------
	virtual void on_button_trigger()
	{
		if (onButtonTrigger)
		{
			onButtonTrigger();
		}
	}

	virtual int on_input(typename _base_type::pointer_type& _e) override
	{
		if (auto r = _base_type::on_input(_e); r > 0)
		{
			return	r;
		}

		auto	mat	= this->node_space_matrix();
		for (auto* e : _e)
		{
			if (auto* key = e->get_keyboard_event(); key)
			{
				if (mKeyCode != key_code::unkown && mKeyCode == key->key)
				{
					if (key->state == input_event::press)
					{
						on_button_trigger();
					}
				}
			}

			if (auto* mouse = e->get_mouse_event(); mouse)
			{
				auto	pos	= vec2<float>(vec3<float>(e->template get_point<float>(), 0) * mat);
				if (mouse->state == input_event::press && this->size().is_inner(pos))
				{
					on_button_trigger();
				}
			}
		}
		return	0;
	}


	key_code			mKeyCode	= key_code::unkown;
};


//! 押しただけで反応するボタン
template<class NodeT>
class	sprite_press_button_node
		: public press_button_node<sprite_node<NodeT>>
{
	using	_base_type	= press_button_node<sprite_node<NodeT>>;

public:
	sprite_press_button_node(g3d::texture* _texture)
		: _base_type(_texture)
	{}
};


AMTRS_NAMESPACE_END
#endif
