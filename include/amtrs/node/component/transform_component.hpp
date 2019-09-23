/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__component__transform_component__hpp
#define	__libamtrs__component__transform_component__hpp
#include "../../graphics/@"
AMTRS_NAMESPACE_BEGIN namespace component {

// ==================================================================
//! component_object に対して３Ｄ座標系を持たせます。
// ------------------------------------------------------------------
//! オブジェクトが hierarchy component を持っている必要があります。
// ------------------------------------------------------------------
template<class T, class Traits>
struct	transform_component
		: public transform<T>
{
	using	_transform_type	= transform<T>;
	using	this_type		= transform_component<T, Traits>;
public:
	using	node_type		= basic_node<T, Traits>;

	using	coordinate		= transform<T>;
	using	value_type		= typename coordinate::value_type;
	using	position_type	= typename coordinate::position_type;
	using	scale_type		= typename coordinate::scale_type;
	using	rotate_type		= typename coordinate::rotate_type;
	using	matrix_type		= typename coordinate::matrix_type;
	using	size_type		= typename coordinate::size_type;
	using	box_type		= typename coordinate::box_type;
	using	bounds_type		= typename coordinate::box_type;

	transform_component(node_type* _owner = nullptr, transform_component* _parent = nullptr)
		: mOwner(_owner)
		, mParentTransform(_parent)
	{}

	node_type* set_position(position_type _pos)
	{
		_transform_type::set_position(_pos);
		return	static_cast<node_type*>(this);
	}


	node_type* set_scale(scale_type _scale)
	{
		_transform_type::set_scale(_scale);
		return	static_cast<node_type*>(this);
	}


	node_type* set_size(size_type _size)
	{
		_transform_type::set_size(_size);
		return	static_cast<node_type*>(this);
	}

	node_type* set_width(value_type _size)
	{
		_transform_type::set_width(_size);
		return	static_cast<node_type*>(this);
	}

	node_type* set_height(value_type _size)
	{
		_transform_type::set_height(_size);
		return	static_cast<node_type*>(this);
	}

	node_type* set_pivot(position_type _pos)
	{
		_transform_type::set_pivot(_pos);
		return	static_cast<node_type*>(this);
	}

	node_type* set_rotate(rotate_type _rotate) noexcept
	{
		_transform_type::set_rotate(_rotate);
		return	static_cast<node_type*>(this);
	}

	using	_transform_type::pivot;
	using	_transform_type::position;
	using	_transform_type::scale;
	using	_transform_type::size;


	// ==============================================================
	//! ローカル座標系を取得します。
	// --------------------------------------------------------------
	const matrix_type& local_matrix() const noexcept
	{
		return	_transform_type::to_matrix();
	}

	// ==============================================================
	//! ワールド座標系を取得します。
	// --------------------------------------------------------------
	const matrix_type& world_matrix() const noexcept
	{
		if (mWorldModified)
		{
			mWorldModified	= false;
			if (parent_transform())
			{
				mWorldMatirx = local_matrix() * parent_transform()->on_children_matrix();
			}
			else
			{
				mWorldMatirx = local_matrix();
			}
		}
		return	mWorldMatirx;
	}

	void set_parent_transform(transform_component* _parent)
	{
		mParentTransform	= _parent;
		on_coordinate_modify();
	}

	bool is_transform_modified() const noexcept { return mWorldModified; }


protected:
	void initialize(node_type* _owner, transform_component* _parent)
	{
		mOwner				= _owner;
		mParentTransform	= _parent;
	}


	transform_component* get_parent_transform()
	{
		return	mParentTransform;
	}

	using	_transform_type::on_size_change;


	// ==============================================================
	//! 子要素のマトリックスを返します。
	// --------------------------------------------------------------
	//! 子要素のスクロール機能などがある場合、このメソッドを上書きする
	//! ことで子要素をスクロールさせることができます。
	// --------------------------------------------------------------
	virtual const matrix_type& on_children_matrix() const noexcept
	{
		return	world_matrix();
	}

	virtual void on_children_transform_change(node_type*)
	{}

	virtual void on_coordinate_modify() override
	{
		_transform_type::on_coordinate_modify();
		update_world();
	}


	void update_world()
	{
		if (mParentTransform)
		{
			mParentTransform->on_children_transform_change(mOwner);
		}
		mWorldModified	= true;
	}


	transform_component* parent_transform() const noexcept { return mParentTransform; }


private:
	mutable bool			mWorldModified		= true;
	mutable matrix_type		mWorldMatirx;
	node_type*				mOwner				= nullptr;
	transform_component*	mParentTransform	= nullptr;
};

} AMTRS_NAMESPACE_END
#endif
