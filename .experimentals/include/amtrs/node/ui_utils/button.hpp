/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__amtrs_utils__ui__button_utils__hpp
#define	__libamtrs__amtrs_utils__ui__button_utils__hpp
#include "def.hpp"
AMTRS_UIUTILS_NAMESPACE_BEGIN

// ============================================================================
//! ボタンとしての機能を持たせるためのコンポーネント
// ----------------------------------------------------------------------------
struct	button
{
public:
	enum class event
	{
		none,
		press,
		cancel,
		fire,
		in,
		out
	};

	void reset()
	{
		capture		= false;
		pressing	= false;
		out			= false;
	}

	// ========================================================================
	//! ボタンが押下状態にあるときに true になります。
	// ------------------------------------------------------------------------
	constexpr bool is_pressing() const noexcept
	{
		return	(this->capture && !this->out)
				? true
				: false;
	}

	template<class NodeT>
	void cancel(NodeT& _node)
	{
		this->capture	= false;
		this->out		= false;
		this->pressing	= false;
	}

	template<class NodeT>
	event input(NodeT& _node, input_event const* _e, vec2<float>* _pos = nullptr)
	{
		vec2<float>	pos	= _pos
							? *_pos
							: vec2<float>(vec3<float>(_e->get_point<float>(), 0) * _node.world_matrix().inverse());
		auto*	event	= _e->get_mouse_event();
		if (event)
		{
			if (!this->capture)
			{
				if ((event->buttons == mouse_event::left) && (event->state == input_event::press))
				{
					if (in_inner(_node.size(), pos))
					{
						// 押された
						this->capture	= true;
						this->out		= false;
						return	event::press;
					}
				}
			}
			else
			{
				if ((event->buttons & mouse_event::left) && (event->state == input_event::release))
				{
					// 離された
					this->capture	= false;
					if (in_inner(_node.size(), pos))
					{
						return	event::fire;
					}
					else
					{
						return	event::cancel;
					}
				}
				else if ((event->buttons & mouse_event::left) && (event->state == input_event::drag))
				{
					if (in_inner(_node.size(), pos))
					{
						if (this->out)
						{
							this->out	= false;
							return	event::in;
						}
					}
					else
					{
						if (!this->out)
						{
							this->out	= true;
							return	event::out;
						}
					}
				}
			}
		}
		return	event::none;
	}

	bool in_inner(const size3<float>& _size, const vec2<float>& _pos)
	{
		vec2<float>		pos	= _pos;
		size3<float>	sz	= _size;
		pos.x		+= expandSize.width /2;
		pos.y		+= expandSize.height/2;
		sz.width	+= expandSize.width;
		sz.height	+= expandSize.height;
		return	sz.is_inner(pos);
	}


	bool				capture		= false;		//!< イベントキャプチャ中なら true
	bool				pressing	= false;		//!< 押下状態なら true
	bool				out			= false;		//!< 
	size2<float>		expandSize	= {0, 0};
};


AMTRS_UIUTILS_NAMESPACE_END
#endif
