/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__fillwrap_node__hpp
#define	__libamtrs__node__fillwrap_node__hpp
#include "padding_node.hpp"
AMTRS_NAMESPACE_BEGIN

// **************************************************************************
//! 親コンテンツを覆うようにします。
// --------------------------------------------------------------------------
//! 親の座標やサイズが変更されたとき、親のサイズに合わせて自身のサイズを変更
//! します。
// --------------------------------------------------------------------------
template<class SuperT>
class	fillwrap_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		fitsize();
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		if (this->parent())
		{
			_size	= this->parent()->children_area();
		}
		super_type::on_size_change(_size);
	}

private:
	void fitsize()
	{
		if (this->parent())
		{
			this->set_size(this->parent()->children_area());
		}
	}
};


// **************************************************************************
//! 水平方向に親コンテンツを覆うようにします。
// --------------------------------------------------------------------------
//! 親の座標やサイズが変更されたとき、親のサイズに合わせて自身のサイズを変更
//! します。
// --------------------------------------------------------------------------
template<class SuperT>
class	horizontalwrap_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		fitsize();
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		if (this->parent())
		{
			_size.width	= this->parent()->children_area().width;
		}
		super_type::on_size_change(_size);
	}

private:
	void fitsize()
	{
		if (this->parent())
		{
			this->set_width(this->parent()->children_area().width);
		}
	}
};


template<class SuperT>
class	verticalwrap_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		fitsize();
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		if (this->parent())
		{
			_size.height	= this->parent()->children_area().height;
		}
		super_type::on_size_change(_size);
	}

private:
	void fitsize()
	{
		if (this->parent())
		{
			this->set_height(this->parent()->children_area().height);
		}
	}
};


// **************************************************************************
//! 自身の位置をもとに、右方向を最大限に広げるようにします。
// --------------------------------------------------------------------------
//! 親の座標やサイズが変更されたとき、親のサイズに合わせて自身のサイズを変更
//! します。
// --------------------------------------------------------------------------
template<class SuperT>
class	rightwrap_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		fitsize();
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		if (this->parent())
		{
			_size.width	= this->parent()->children_area().width - this->position().x;
		}
		super_type::on_size_change(_size);
	}

private:
	void fitsize()
	{
		if (this->parent())
		{
			this->set_width(this->parent()->children_area().width - this->position().x);
		}
	}
};


// **************************************************************************
//! 自身の位置をもとに、下方向を最大限に広げるようにします。
// --------------------------------------------------------------------------
//! 親の座標やサイズが変更されたとき、親のサイズに合わせて自身のサイズを変更
//! します。
// --------------------------------------------------------------------------
template<class SuperT>
class	bottomwrap_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		fitsize();
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		if (this->parent())
		{
			_size.height	= this->parent()->children_area().height - this->position().y;
		}
		super_type::on_size_change(_size);
	}

private:
	void fitsize()
	{
		if (this->parent())
		{
			this->set_height(this->parent()->children_area().height - this->position().y);
		}
	}
};

// **************************************************************************
//! 子コンテンツを覆うようにします。
// --------------------------------------------------------------------------
//! 子の位置やサイズが変更されると自身のサイズを変更します。
//! 子がfillrwapなど親のサイズを見るような構造になっていると予期せぬ動作を
//! する場合があります。
// --------------------------------------------------------------------------
template<class SuperT>
class	children_wrap_node
		: public padding_node<SuperT>
{
public:
	using	super_type				= padding_node<SuperT>;
	using	transform_component		= typename super_type::transform_component;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		fitsize();
	}

	void on_children_transform_change(transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		fitsize();
	}

	void on_children_size_change(transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		fitsize();
	}

private:
	void fitsize()
	{
		if (!mFitting)
		{
			mFitting	= true;
			this->schedule_once([this](float)
			{
				auto	cb	= super_type::children_bounds();
				auto	pad	= super_type::get_padding();
				this->set_size(cb.size() + size3<float>(pad.x.minimam + pad.x.maximam, pad.y.minimam + pad.y.maximam, pad.z.minimam + pad.z.maximam));
				this->visit_children([&](auto c)
				{
					c->set_position(c->position() + cb.position());
					return	false;
				}, false);
				mFitting	= false;
			});
		}
	}

	bool	mFitting	= false;
};



// **************************************************************************
//! 子コンテンツを覆うようにします。
// --------------------------------------------------------------------------
//! 子の位置やサイズが変更されると自身のサイズを変更します。
//! 子がfillrwapなど親のサイズを見るような構造になっていると予期せぬ動作を
//! する場合があります。
// --------------------------------------------------------------------------
template<class SuperT>
class	children_horaizontalwrap_node
		: public padding_node<SuperT>
{
public:
	using	super_type				= padding_node<SuperT>;
	using	transform_component		= typename super_type::transform_component;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		fitsize();
	}

	void on_children_transform_change(transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		fitsize();
	}

	void on_children_size_change(transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		fitsize();
	}

private:
	void fitsize()
	{
		if (!mFitting)
		{
			mFitting	= true;
			this->schedule_once([this](float)
			{
				auto	cb	= super_type::children_bounds();
				auto	pad	= super_type::get_padding();
				this->set_width(cb.size().width + pad.x.minimam + pad.x.maximam);
				this->visit_children([&](auto c)
				{
					auto	pos	= c->position();
					pos.x	-= cb.position().x;
					c->set_position(pos);
					return	false;
				}, false);
				mFitting	= false;
			});
		}
	}

	bool	mFitting	= false;
};



// **************************************************************************
//! 子コンテンツを覆うようにします。
// --------------------------------------------------------------------------
//! 子の位置やサイズが変更されると自身のサイズを変更します。
//! 子がfillrwapなど親のサイズを見るような構造になっていると予期せぬ動作を
//! する場合があります。
// --------------------------------------------------------------------------
template<class SuperT>
class	children_verticalwrap_node
		: public padding_node<SuperT>
{
public:
	using	super_type				= padding_node<SuperT>;
	using	transform_component		= typename super_type::transform_component;
	using	super_type::super_type;

protected:
	void on_enter() override
	{
		super_type::on_enter();
		fitsize();
	}

	void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		fitsize();
	}

	void on_children_transform_change(transform_component* _child) override
	{
		super_type::on_children_transform_change(_child);
		fitsize();
	}

	void on_children_size_change(transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		fitsize();
	}

private:
	void fitsize()
	{
		if (!mFitting)
		{
			mFitting	= true;
			this->schedule_once([this](float)
			{
				auto	cb	= super_type::children_bounds();
				auto	pad	= super_type::get_padding();
				this->set_height(cb.size().height + pad.y.minimam + pad.y.maximam);
				this->visit_children([&](auto c)
				{
					auto	pos	= c->position();
					pos.y	-= cb.position().y;
					c->set_position(pos);
					return	false;
				}, false);
				mFitting	= false;
			});
		}
	}

	bool	mFitting	= false;
};

AMTRS_NAMESPACE_END
#endif
