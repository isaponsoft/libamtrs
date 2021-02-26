/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__linerlayout_node__hpp
#define	__libamtrs__node__linerlayout_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN



// **************************************************************************
//! layoutchild を持っている子ノードを線形に並べていきます。
// --------------------------------------------------------------------------
//! デフォルトは横方向です。
// --------------------------------------------------------------------------
template<class SuperT>
class	linerlayout_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	node_type		= typename super_type::node_type;
	using	size_type		= typename super_type::size_type;

	enum class	origin_type
	{
		horizontal		= 0,
		vertical		= 1,
	};
	static constexpr origin_type	horizontal	= origin_type::horizontal;
	static constexpr origin_type	vertical	= origin_type::vertical;


	using	super_type::super_type;


	template<class T>
	T* add_item(T* _item)
	{
		this->_add_item(_item);
		return	_item;
	}

	origin_type origin() const noexcept
	{
		return	mOrigin;
	}

	void set_origin(origin_type _origin)
	{
		if (mOrigin != _origin)
		{
			mOrigin	= _origin;
			reposition(this->size());
		}
	}

	void set_spacing(size_type _size)
	{
		mSpacing	= _size;
		reposition(this->size());
	}

	void reposition(typename super_type::size_type const& _size)
	{
		vec3<float>		pos	= {0, 0, 0};
		size3<float>	msz	= {0, 0, 0};
		if (mOrigin == horizontal)
		{
			for (auto c : mItems)
			{
				auto	rpos	= pos;
				rpos.x	+= c->size().width;
				if (rpos.x > _size.width)
				{
					pos.x	=  0;
					pos.y	+= msz.height + mSpacing.height;
					msz		= {0, 0, 0};
				}
				c->set_position(pos);
				pos.x		+= c->size().width;
				msz.height	=  std::max(msz.height, c->size().height);
			}
		}
		else
		{
			for (auto c : mItems)
			{
				auto	rpos	= pos;
				rpos.y	+= c->size().height;
				if (rpos.y > _size.height)
				{
					pos.y	=  0;
					pos.x	+= msz.width + mSpacing.width;
					msz		= {0, 0, 0};
				}
				c->set_position(pos);
				pos.y		+= c->size().height;
				msz.width	=  std::max(msz.width, c->size().width);
			}
		}
	}

	auto const& items() const noexcept { return mItems; }

protected:
	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		reposition(_size);
	}

	virtual void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		if (_event == super_type::on_child_event::remove)
		{
			if (auto it = std::find(mItems.begin(), mItems.end(), _child); it != mItems.end())
			{
				mItems.erase(it);
			}
		}
		reposition(this->size());
	}

	virtual void on_children_transform_change(typename super_type::transform_component* _transform) override
	{
		super_type::on_children_transform_change(_transform);
		reposition(this->size());
	}

private:
	void _add_item(node_type* _item)
	{
		mItems.push_back(_item);
		this->add_child(_item);
	}


	origin_type				mOrigin		= origin_type::horizontal;
	size_type				mSpacing	= {0, 0, 0};				// 要素と要素の間の空白
	std::vector<node_type*>	mItems;
};

AMTRS_NAMESPACE_END
#endif
