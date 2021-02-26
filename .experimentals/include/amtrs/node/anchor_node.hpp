/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__anchor_node__hpp
#define	__libamtrs__node__anchor_node__hpp
#include "../type_traits/strongtypedef.hpp"
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN
AMTRS_IMPLEMENTS_BEGIN(anchor_node)
struct	anchor_types : public strongtypedef<float>
{
	using	super_type	= strongtypedef<float>;
	using	super_type::super_type;
	using	super_type::operator =;
	using	super_type::operator ==;
	using	super_type::operator !=;
	constexpr anchor_types(super_type const& _r) : super_type(_r) {}

	constexpr static super_type		none	= std::numeric_limits<float>::infinity();
	constexpr static super_type		minimam	= 0.0f;
	constexpr static super_type		center	= 0.5f;
	constexpr static super_type		maximam	= 1.0f;
};
AMTRS_IMPLEMENTS_END(anchor_node)

struct	anchor_node_base
{
	using	anchor_types	= AMTRS_IMPLEMENTS(anchor_node)::anchor_types;

	static constexpr anchor_types	none	= anchor_types::none;
	static constexpr anchor_types	minimam	= anchor_types::minimam;
	static constexpr anchor_types	center	= anchor_types::center;
	static constexpr anchor_types	maximam	= anchor_types::maximam;

	// 別名
	static constexpr anchor_types	anchor_left		= anchor_types::minimam;
	static constexpr anchor_types	anchor_center	= anchor_types::center;
	static constexpr anchor_types	anchor_right	= anchor_types::maximam;
	static constexpr anchor_types	anchor_top		= anchor_types::minimam;
	static constexpr anchor_types	anchor_middle	= anchor_types::center;
	static constexpr anchor_types	anchor_bottom	= anchor_types::maximam;
};


// **************************************************************************
//! 親コンテンツに対して指定した位置に表示します。
// --------------------------------------------------------------------------
//! 親の座標やサイズが変更されたとき、親のサイズに合わせて自身の位置を変更
//! します。
// --------------------------------------------------------------------------
template<class BaseT>
class	anchor_node
		: public BaseT
		, public anchor_node_base
{
public:
	using	super_type		= BaseT;
	using	super_type::super_type;


	vec3<anchor_types> get_anchor() const noexcept { return mAnchor; }

	void set_anchor(vec3<anchor_types> _anchor)
	{
		if (mAnchor != _anchor)
		{
			mAnchor	= _anchor;
			anchor_reposition();
		}
	}

	void anchor_reposition()
	{
		if (this->parent())
		{
			vec3<float>		pos	= this->position();
			auto	cb	= this->parent()->children_area();
			pos.x	= mAnchor.x == none ? pos.x : cb.width  * mAnchor.x;
			pos.y	= mAnchor.y == none ? pos.y : cb.height * mAnchor.y;
			pos.z	= mAnchor.z == none ? pos.z : cb.depth  * mAnchor.z;
			this->set_position(pos);
		}
	}


protected:
	void on_coordinate_modify() override
	{
		super_type::on_coordinate_modify();
		anchor_reposition();
	}

private:

	vec3<anchor_types>	mAnchor	= {none, none, none};
};



AMTRS_NAMESPACE_END
#endif
