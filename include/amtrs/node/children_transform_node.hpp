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
	using	_base_type		= NodeT;
public:
	using	value_type		= typename _base_type::value_type;
	using	coordinate		= typename _base_type::coordinate;
	using	matrix_type		= typename _base_type::matrix_type;
	using	scale_type		= typename _base_type::scale_type;
	using	rotate_type		= typename _base_type::rotate_type;
	using	position_type	= typename _base_type::position_type;
	using	size_type		= typename _base_type::size_type;

	virtual const matrix_type& world_matrix() const noexcept override
	{
		if (mMatrixUpdate)
		{
			mMatrixUpdate	= false;
			mMatrix = _base_type::world_matrix() * mCoordinate.to_matrix();
		}
		return	mMatrix;
	}

protected:
	children_transform_node()
		: mCoordinate(this)
	{}

	const scale_type& children_pivot() const noexcept { return mCoordinate.pivot(); }
	void set_children_pivot(scale_type _position) noexcept;

	position_type children_position() const noexcept { return mCoordinate.position(); }
	void set_children_position(position_type _position) noexcept;
	void set_children_position_x(value_type _position)  noexcept;
	void set_children_position_y(value_type _position)  noexcept;
	void set_children_position_z(value_type _position)  noexcept;

	const scale_type& children_scale() const noexcept { return mCoordinate.scale(); }
	void set_children_scale(scale_type _scale) noexcept;

	const rotate_type& children_rotate() const noexcept { return mCoordinate.rotate(); }
	void set_children_rotate(rotate_type _rotate) noexcept;

	size_type children_size() const noexcept { return mCoordinate.size(); }
	void set_children_size(size_type _size) noexcept;
	void set_children_width(value_type _width)   noexcept;
	void set_children_height(value_type _height) noexcept;


	      coordinate* children_transform()       noexcept { return &mCoordinate; }
	const coordinate* children_transform() const noexcept { return &mCoordinate; }


	virtual void on_coordinate_modify() noexcept override
	{
		_base_type::on_coordinate_modify();
		mMatrixUpdate	= true;
	}


private:
	class	local_transform
			: public _base_type::coordinate
	{
	public:
		local_transform(children_transform_node* _owner)
			: mOwner(_owner)
		{}

		virtual void on_coordinate_modify() noexcept override
		{
			_base_type::coordinate::on_coordinate_modify();
			mOwner->on_coordinate_modify();
		}

	private:
		children_transform_node*	mOwner;
	};

	local_transform				mCoordinate;
	mutable bool				mMatrixUpdate	= false;
	mutable matrix_type			mMatrix			= matrix_type::identify();
};


template<class NodeT>
void children_transform_node<NodeT>::set_children_pivot(scale_type _position) noexcept
{
	if (mCoordinate.pivot() != _position)
	{
		mCoordinate.set_pivot(_position);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_position(position_type _position) noexcept
{
	if (mCoordinate.position() != _position)
	{
		mCoordinate.set_position (_position);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_position_x(value_type _position)  noexcept
{
	if (mCoordinate.position().x != _position)
	{
		mCoordinate.set_position_x(_position);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_position_y(value_type _position)  noexcept
{
	if (mCoordinate.position().y != _position)
	{
		mCoordinate.set_position_y(_position);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_position_z(value_type _position)  noexcept
{
	if (mCoordinate.position().z != _position)
	{
		mCoordinate.set_position_z(_position);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_scale(scale_type _scale) noexcept
{
	if (mCoordinate.scale() != _scale)
	{
		mCoordinate.set_scale(_scale);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_rotate(rotate_type _rotate) noexcept
{
	if (mCoordinate.rotate() != _rotate)
	{
		mCoordinate.set_rotate(_rotate);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_size(size_type _size) noexcept
{
	if (mCoordinate.size() != _size)
	{
		mCoordinate.set_size(_size);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_width(value_type _width) noexcept
{
	if (mCoordinate.size().width != _width)
	{
		mCoordinate.set_width(_width);
		on_coordinate_modify();
	}
}


template<class NodeT>
void children_transform_node<NodeT>::set_children_height(value_type _height) noexcept
{
	if (mCoordinate.size().height != _height)
	{
		mCoordinate.set_height(_height);
		on_coordinate_modify();
	}
}


AMTRS_NAMESPACE_END
#endif
