/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__padding_node__hpp
#define	__libamtrs__node__padding_node__hpp
#include "../type_traits/minmax.hpp"
#include "../node.hpp"
AMTRS_NAMESPACE_BEGIN

// ============================================================================
//! 子ノードに対してパディングを行います。
// ----------------------------------------------------------------------------
template<class NodeT>
class	padding_node
		: public NodeT
{
public:
	using	super_type		= NodeT;
	using	position_type	= typename super_type::position_type;
	using	box_type		= typename super_type::box_type;
	using	size_type		= typename super_type::size_type;
	using	matrix_type		= typename super_type::matrix_type;
	using	minmax_type		= minmax<typename size_type::value_type>;

	using	super_type::super_type;


	void set_padding(size_type _ltf, size_type _rbd)
	{
		if (mLTF != _ltf || mRBD != _rbd)
		{
			mLTF	= _ltf;
			mRBD	= _rbd;
			on_coordinate_modify();
		}
	}

	vec3<minmax_type> get_padding() const noexcept;


	virtual matrix_type children_matrix() const override
	{
		if (mModified)
		{
			mModified	= false;
			mChildrenTransform	= matrix_type::translate(position_type(mLTF));
		}
		return	mChildrenTransform * super_type::children_matrix();
	}


	virtual box_type children_area() const override
	{
		box_type	retval({0,0,0}, this->size());
		retval.x		=  mLTF.width;
		retval.y		=  mLTF.height;
		retval.z		=  mLTF.depth;
		retval.width	-= mLTF.width  + mRBD.width;
		retval.height	-= mLTF.height + mRBD.height;
		retval.depth	-= mLTF.depth  + mRBD.depth;
		return	retval;
	}


protected:
	virtual void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		super_type::rerender_request();
		mModified	= true;
	}

private:
	size_type				mLTF	= {0, 0, 0};
	size_type				mRBD	= {0, 0, 0};
	mutable bool			mModified			= false;
	mutable matrix_type		mChildrenTransform	= matrix_type::identify();
};


template<class NodeT>
auto padding_node<NodeT>::get_padding() const noexcept -> vec3<minmax_type>
{
	return	{
		{ mLTF.width , mRBD.width  },
		{ mLTF.height, mRBD.height },
		{ mLTF.depth , mRBD.depth  }
	};
}


AMTRS_NAMESPACE_END
#endif
