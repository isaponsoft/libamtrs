/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__button_sprite_node__hpp
#define	__libamtrs__node__button_sprite_node__hpp
#include "button_node.hpp"
#include "sprite_node.hpp"
AMTRS_NAMESPACE_BEGIN


template<class NodeT>
class	button_sprite_node
		: public button_node<sprite_node<NodeT>>
{
	using	_base_type		= button_node<sprite_node<NodeT>>;
public:

	enum class	sprite_split_type
	{
		none		= 0,		//!< テクスチャは分割しない
		vertical	= 1,		//!< 縦方向に分割し、押下中は２枚目の画像を使う
		horizontal	= 2,		//!< 横方向に分割す、押下中は２枚目の画像を使う
	};

	static constexpr sprite_split_type		split_none			= sprite_split_type::none;
	static constexpr sprite_split_type		split_vertical		= sprite_split_type::vertical;
	static constexpr sprite_split_type		split_horizontal	= sprite_split_type::horizontal;

	using	node_type		= typename NodeT::node_type;

	button_sprite_node(g3d::texture* _image, sprite_split_type _split = split_none)
		: mSplitType(_split)
	{
		set_texture(_image);
	}

	void set_texture(g3d::texture* _image)
	{
		switch (mSplitType)
		{
			case split_none :
			{
				mTexRelease	= _image;
				mTexPress	= _image;
				break;
			}

			case split_vertical :
			{
				auto	ts	= _image->size();
				mTexRelease	= new g3d::sub_texture(_image, {0, 0}, {ts.width, ts.height/2});
				mTexPress	= new g3d::sub_texture(_image, {0, ts.height/2}, {ts.width, ts.height/2});
				break;
			}

			case split_horizontal :
			{
				auto	ts	= _image->size();
				mTexRelease	= new g3d::sub_texture(_image, {0, 0}, {ts.width/2, ts.height});
				mTexPress	= new g3d::sub_texture(_image, {ts.width/2, 0}, {ts.width/2, ts.height});
				break;
			}
		}
		_base_type::set_texture(_base_type::is_button_pressing() ? mTexPress : mTexRelease);
	}


protected:
	virtual void on_change_press_state(bool _press) override
	{
		_base_type::on_change_press_state(_press);
		_base_type::set_texture(_base_type::is_button_pressing() ? mTexPress : mTexRelease);
	}

	sprite_split_type	mSplitType	= split_none;
	ref<g3d::texture>	mTexRelease;
	ref<g3d::texture>	mTexPress;
};


AMTRS_NAMESPACE_END
#endif
