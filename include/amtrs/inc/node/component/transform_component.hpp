/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__component__transform_component__hpp
#define	__libamtrs__component__transform_component__hpp
#include "../../../graphics.hpp"
AMTRS_NAMESPACE_BEGIN namespace component {

// ==================================================================
//! component_object に対して３Ｄ座標系を持たせます。
// ------------------------------------------------------------------
//! オブジェクトが hierarchy component を持っている必要があります。
// ------------------------------------------------------------------
template<class NodeT>
struct	transform_component
		: public transform<float>
{
	using	_node_type		= NodeT;
	using	_transform_type	= transform<float>;
public:
	using	coordinate		= transform<float>;
	using	value_type		= typename coordinate::value_type;
	using	position_type	= typename coordinate::position_type;
	using	scale_type		= typename coordinate::scale_type;
	using	rotate_type		= typename coordinate::rotate_type;
	using	matrix_type		= typename coordinate::matrix_type;
	using	size_type		= typename coordinate::size_type;
	using	box_type		= typename coordinate::box_type;

	transform_component(NodeT* _owner, transform_component* _parent = nullptr)
		: mOwner(_owner)
		, mParentTransform(_parent)
	{}

	NodeT* set_position(position_type _pos)
	{
		if (_transform_type::set_position(_pos) && mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		return	static_cast<NodeT*>(this);
	}


	NodeT* set_size(size_type _size)
	{
		if (_transform_type::set_size(_size) && mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		return	static_cast<NodeT*>(this);
	}

	NodeT* set_width(value_type _size)
	{
		if (_transform_type::set_width(_size) && mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		return	static_cast<NodeT*>(this);
	}

	NodeT* set_height(value_type _size)
	{
		if (_transform_type::set_height(_size) && mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		return	static_cast<NodeT*>(this);
	}

	NodeT* set_pivot(position_type _pos)
	{
		if (_transform_type::set_pivot(_pos) && mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		return	static_cast<NodeT*>(this);
	}

	NodeT* set_rotate(rotate_type _rotate) noexcept
	{
		if (_transform_type::set_rotate(_rotate) && mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		return	static_cast<NodeT*>(this);
	}


	//! 自分自身のマトリックスを取得します。
	const matrix_type& render_matrix() const noexcept
	{
		if (mWorldModified)
		{
			mWorldModified	= false;
			if (parent_transform())
			{
				mWorldMatirx = _transform_type::to_matrix() * parent_transform()->world_matrix();
			}
			else
			{
				mWorldMatirx = _transform_type::to_matrix();
			}
		}
		return	mWorldMatirx;
	}


	virtual const matrix_type& world_matrix() const noexcept
	{
		return	render_matrix();
	}

	//! ローカル座標系に変換するためのマトリックスを生成します。
	matrix_type node_space_matrix() const noexcept
	{
		return	world_matrix().inverse();
	}

	transform_component* get_parent_transform()
	{
		return	mParentTransform;
	}

	void set_parent_transform(transform_component* _parent)
	{
		mParentTransform	= _parent;
		update_world_matrix();
	}

	bool is_transform_modified() const noexcept { return mWorldModified; }

	void coordinate_modify() noexcept { on_coordinate_modify(); }

protected:
	virtual void on_children_transform_change(NodeT*)
	{}

	virtual void on_coordinate_modify() override
	{
		_transform_type::on_coordinate_modify();
		update_world_matrix();
	}


	void update_world_matrix() const noexcept
	{
		mWorldModified	= true;
		mOwner->visit_children([](auto* c) -> bool
		{
			static_cast<transform_component<NodeT>*>(c)->on_coordinate_modify();
			return	false;
		});
	}

	transform_component* parent_transform() const noexcept { return mParentTransform; }


private:
	mutable bool			mWorldModified		= true;
	mutable matrix_type		mWorldMatirx;
	NodeT*					mOwner				= nullptr;
	transform_component*	mParentTransform	= nullptr;
};

} AMTRS_NAMESPACE_END
#endif
