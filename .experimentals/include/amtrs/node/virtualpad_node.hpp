/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__virtualpad_node__hpp
#define	__libamtrs__node__virtualpad_node__hpp
#include <map>
#include "../input/@"
#include "node.hpp"
#include "sprite_node.hpp"
AMTRS_NAMESPACE_BEGIN

#if	0
// ============================================================================
//! 
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
class	virtualpad_node
		: public node
{
	using	_base_type			= node;
public:
	using	button_node_type	= press_button_node<sprite_node<node>>;

	struct	button_data
	{
		button_node_type*	buttn;
		uint32_t			gamepad	= 0;
	};


	static ref<virtualpad_node> create(g3d::texture* _bg, g3d::texture* _cursor)
	{
		ref<virtualpad_node>	thiz = new virtualpad_node();
		thiz->initialize(_bg, _cursor);
		return	thiz;
	}

	template<class Callback>
	void add_button(int _button_id, g3d::texture* _texture, Callback&& _callback)
	{
		ref<button_node_type>	btn	= new button_node_type(_texture);
		btn->set_pivot({0.5f, 0.5f, 0});
		btn->onButtonTrigger	= std::forward<Callback>(_callback);

btn->set_position({500 - float(_button_id * 80), 0, 0});
		add_child(btn);

		mButtons.insert({_button_id, {btn.get(), 0}});
	}

	void set_gamepad_button(int _button_id, uint32_t _button_mask)
	{
		mButtons[_button_id].gamepad	= _button_mask;
	}


/*
		if (mGamepad)
		{
			if (mGamepad->buttons() & gamepad::dpad_b)
			{
				mPlayer->jump();
			}
		}
*/


	//! ゲームパッドを関連付けします。
	void set_gamepad(gamepad* _gamepad) noexcept
	{
		mGamepad	= _gamepad;
	}

	gamepad* get_gamepad() const noexcept
	{
		return	mGamepad.get();
	}

	virtual int on_input(typename _base_type::pointer_event& _e) override
	{
		if (auto r = node::on_input(_e); r > 0)
		{
			return	r;
		}

		auto	mat	= this->node_space_matrix();
		for (auto* e : _e)
		{
			auto*	key		= e->get_keyboard_event();
			if (key)
			{
				if (key->state == input_event::press)
				{
					mHaveInput	= true;
					if (key->key == key_code::left_key)		{ mX	= -1;  mHaveInput	= true; }
					if (key->key == key_code::right_key)	{ mX	=  1;  mHaveInput	= true; }
					if (key->key == key_code::up_key)		{ mY	= -1;  mHaveInput	= true; }
					if (key->key == key_code::down_key)		{ mY	=  1;  mHaveInput	= true; }
				}
				else if (key->state == input_event::release)
				{
					if (key->key == key_code::left_key)		{ mX	= 0;   mHaveInput	= true; }
					if (key->key == key_code::right_key)	{ mX	= 0;   mHaveInput	= true; }
					if (key->key == key_code::up_key)		{ mY	= 0;   mHaveInput	= true; }
					if (key->key == key_code::down_key)		{ mY	= 0;   mHaveInput	= true; }
				}
			}

			auto*	event	= e->get_mouse_event();
			if (event)
			{
				auto	pos		= vec2<float>(vec3<float>(e->get_point<float>(), 0) * mat);
				if ((event->buttons & mouse_event::left) && (event->state == input_event::press))
				{
					if (mBg->size().is_inner(pos + vec2<float>(vec3<float>(mBg->size()/2))))
					{
						// 押された
						mPressing	= true;
						mHaveInput	= true;
					}
				}
				else if ((event->buttons & mouse_event::left) && (event->state == input_event::release))
				{
					// 離された
					mPressing	= false;
					mX			= 0;
					mY			= 0;
				}
				else if ((event->buttons & mouse_event::left) && (event->state == input_event::drag))
				{
					// 動かした
					if (mPressing)
					{
						auto	angle	= atan2(pos.x, -pos.y);
						auto	way_x	= sinf(angle);
						auto	way_y	= -cosf(angle);
						mX			= way_x;
						mY			= way_y;
						mHaveInput	= true;

						#if 0
						// 単なるログ
						{
							auto	log_a	= radians_to_degrees(angle);
							if (log_a < 0) log_a +=  + 360.0f;
							AMTRS_DEBUG_LOG("%f %f %f", way_x, way_y, log_a);
						}
						#endif
					}
				}
			}
		}
		return	0;
	}

	virtual void on_update(float _delta) override
	{
		node::on_update(_delta);
		if (mGamepad)
		{
			mGamepad->update();
			auto	buttons	= mGamepad->buttons();
			for (auto& b : mButtons)
			{
				if (b.second.gamepad & buttons)
				{
					b.second.buttn->onButtonTrigger();
				}
			}

			if (buttons & gamepad::dpad_left)
			{
				mX	= -1;
			}
			else if (buttons & gamepad::dpad_right)
			{
				mX	= 1;
			}
			else
			{
				if (!mPressing)
				{
					mX	= 0;
				}
			}

			if (buttons & gamepad::dpad_up)
			{
				mY	= -1;
			}
			else if (buttons & gamepad::dpad_down)
			{
				mY	= 1;
			}
			else
			{
				if (!mPressing)
				{
					mY	= 0;
				}
			}
		}

		auto	sz	= mBg->size() / 2.0f;
		mCursor->set_position(vec3<float>(sz * vec3<float>(mX, mY, 1) * 0.5f));

		mHaveInput	= false;
	}

	
	void initialize(g3d::texture* _bg, g3d::texture* _cursor)
	{
		{
			mCursor		= new sprite_node<node>(_cursor);
			mCursor->set_pivot({0.5f, 0.5f, 0});
			add_child(mCursor);
		}
		{
			mBg		= new sprite_node<node>(_bg);
			mBg->set_pivot({0.5f, 0.5f, 0});
			add_child(mBg);
		}
	}

	bool			mHaveInput	= false;
	bool			mPressing	= false;
	float			mX			= 0;
	float			mY			= 0;

	ref<gamepad>	mGamepad;

	sprite_node<node>*			mBg;
	sprite_node<node>*			mCursor;
	std::map<int, button_data>	mButtons;
};
#endif

AMTRS_NAMESPACE_END
#endif
