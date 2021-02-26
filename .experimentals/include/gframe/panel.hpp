/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__gframe__panel__hpp
#define	__gframe__panel__hpp
#include <amtrs/g3d/texture.hpp>
#include <amtrs/node/children_transform_node.hpp>
#include <amtrs/node/linerlayout_node.hpp>
#include <amtrs/node/sprite_node.hpp>
#include "module.hpp"
GFRAME_NAMESPACE_BEGIN
using namespace amtrs;

// ****************************************************************************
//! パネル。ヘッダーやフッター、サイドバーの基本
// ----------------------------------------------------------------------------
class	panel : public linerlayout_node<children_transform_node<sprite_node<node>>>
{
public:
	using	super_type		= linerlayout_node<children_transform_node<sprite_node<node>>>;
	using	size_type		= typename super_type::size_type;
	using	selector_type	= std::string;

	//! 配置場所
	enum class	side_type
	{
		fullsize	= 0,		//!< 親ノード全体を覆うようにする
		top_side	= 1,		//!< 親ノードの下に配置する
		bottom_side	= 2,		//!< 親ノードの下に配置する
		left_side	= 3,		//!< 親ノードの左に配置する
		right_side	= 4,		//!< 親ノードの右に配置する
	};

	static constexpr side_type		fullsize			= side_type::fullsize;
	static constexpr side_type		top_side			= side_type::top_side;
	static constexpr side_type		bottom_side			= side_type::bottom_side;
	static constexpr side_type		left_side			= side_type::left_side;
	static constexpr side_type		right_side			= side_type::right_side;


	static constexpr int			default_priority	= 100;
	static constexpr float			visible_druction	= 0.12f;

	panel();
	virtual ~panel();

	virtual void hidden();
	virtual void show();

	virtual void set_side(side_type _side);

	virtual void set_bg(std::string const& _imagename);

protected:
	virtual void on_coordinate_modify() override;
	virtual void on_size_change(typename super_type::size_type& _size) override;
	virtual void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override;
	virtual void on_enter() override;

	virtual void update_position();

	side_type	mSide			= bottom_side;
	bool		mScheduleRepos	= false;
	bool		mHidden			= false;
};


GFRAME_NAMESPACE_END
#endif
