/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__children_transform_node__hpp
#define	__libamtrs__node__children_transform_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! 子ノードに対して変形を行えるようにします。
// ----------------------------------------------------------------------------
//! このテンプレートを継承することで、派生クラスは children_xxxx() という関数を
//! 利用できます。これらの関数を使うことで子ノードの位置やスケールを変更できます。
// ----------------------------------------------------------------------------
template<class NodeT>
class	children_transform_node
		: public NodeT
{
public:
	using	super_type		= NodeT;
	using	value_type		= typename super_type::value_type;
	using	matrix_type		= typename super_type::matrix_type;
	using	scale_type		= typename super_type::scale_type;
	using	rotate_type		= typename super_type::rotate_type;
	using	position_type	= typename super_type::position_type;
	using	size_type		= typename super_type::size_type;


	children_transform_node()
		: mChildrenTransform(this)
	{}

	const scale_type& children_pivot() const noexcept { return mChildrenTransform.pivot(); }
	void set_children_pivot(scale_type _position) noexcept { mChildrenTransform.set_pivot(_position); }

	size_type children_size() const noexcept { return mChildrenTransform.size(); }
	void set_children_size(size_type _size) noexcept { mChildrenSizes = true; mChildrenTransform.set_size(_size); }

	position_type children_position() const noexcept { return mChildrenTransform.position(); }
	void set_children_position(position_type _position) noexcept { mChildrenTransform.set_position(_position); }
	void set_children_position_x(value_type _position)  noexcept { mChildrenTransform.set_position_x(_position); }
	void set_children_position_y(value_type _position)  noexcept { mChildrenTransform.set_position_y(_position); }
	void set_children_position_z(value_type _position)  noexcept { mChildrenTransform.set_position_z(_position); }

	const scale_type& children_scale() const noexcept { return mChildrenTransform.scale(); }
	void set_children_scale(scale_type _scale) noexcept { mChildrenTransform.set_scale(_scale); }

	const rotate_type& children_rotate() const noexcept { return mChildrenTransform.rotate(); }
	void set_children_rotate(rotate_type _rotate) noexcept { mChildrenTransform.set_rotate(_rotate); }

	transform<float>*       children_transform()       noexcept { return &mChildrenTransform; }
	transform<float> const* children_transform() const noexcept { return &mChildrenTransform; }


protected:
	virtual void on_size_change(size_type& _size) override
	{
		super_type::on_size_change(_size);
		if (!mChildrenSizes)
		{
			mChildrenTransform.set_size(_size);
		}
	}

	virtual matrix_type children_matrix() const override
	{
		return	mChildrenTransform.to_matrix() * super_type::children_matrix();
	}

private:
	class	local_transform
			: public transform<float>
	{
	public:
		local_transform(children_transform_node* _owner)
			: mOwner(_owner)
		{}

		virtual void on_coordinate_modify() override
		{
			transform<float>::on_coordinate_modify();
			mOwner->on_coordinate_modify();
			super_type::rerender_request();
		}

	private:
		children_transform_node*	mOwner;
	};

	local_transform				mChildrenTransform;
	bool						mChildrenSizes		= false;
};



AMTRS_NAMESPACE_END
#endif
