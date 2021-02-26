/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__flexbox_node__hpp
#define	__libamtrs__node__flexbox_node__hpp
#include "itemable_node.hpp"
AMTRS_NAMESPACE_BEGIN


struct	flexbox_node_base
{
	enum class	direction_type
	{
		row				= 0,		//!< 子要素を左から右に配置
		row_reverse		= 1,		//!< 子要素を右から左に配置
		column 			= 2,		//!< 子要素を上から下に配置
		column_reverse	= 3,		//!< 子要素を下から上に配置
	};


	enum class	justify_content_type
	{
		start			= 0,		//!< 行の開始入りから
		end				= 1,		//!< 行の終了位置から
		center			= 2,		//!< 中央ぞろえ
		space_between	= 3,		//!< 最初と最後の子要素を両端へ、残りを均等に
		space_around	= 4,		//!< 両端の子要素も含め、均等に
	};

	enum class	wrap_type
	{
		nowrap			= 0,		//!< 子要素を折り返しせず、一行に並べる
		wrap			= 1,		//!< (初期値)子要素を折り返し、複数行に上から下へ並べる
		wrap_reverse	= 2,		//!< 子要素を折り返し、複数行に下から上へ並べる
	};


	enum class	align_self_type
	{
		stretch			= 0,		//!< 縦幅を広げる
		top				= 1,		//!< （初期値）上に合わせる
		bottom			= 2,		//!< 下に合わせる
		center			= 3,		//!< 中央に合わせる
	};

};



// ****************************************************************************
//! HTMLのフレックスボックスに似た動作を行うノードです。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
template<class SuperT, class Traits = flexbox_node_traits<SuperT>>
class	flexbox_node
		: public itemable_node<SuperT, Traits>
		, public flexbox_node_base
{
public:
	using	super_type				= itemable_node<SuperT, Traits>;
	using	node_type				= typename super_type::node_type;
	using	direction_type			= flexbox_node_base::direction_type;
	using	justify_content_type	= flexbox_node_base::justify_content_type;
	using	align_self_type			= flexbox_node_base::align_self_type;
	using	wrap_type				= flexbox_node_base::wrap_type;
	using	traits_type				= Traits;

	using	super_type::super_type;

	void set_align_self(align_self_type _value)
	{
		mAlignSelft	= _value;
		reposition();
	}

	void set_direction(direction_type _direction)
	{
		mDirection	= _direction;
		reposition();
	}

	void set_justify(justify_content_type _justify)
	{
		mJustify	= _justify;
		reposition();
	}

	void set_spacing(vec2<float> _spacing)
	{
		mSpacing	= _spacing;
		reposition();
	}

	void reposition()
	{
		if (!mReposition)
		{
			mReposition	= true;
			this->schedule_once([this](float)
			{
				on_reposition();
				mReposition	= false;
			});
		}
	}

protected:
	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		reposition();
	}

	virtual void on_child(typename super_type::node_type* _child, typename super_type::on_child_event _event) override
	{
		super_type::on_child(_child, _event);
		reposition();
	}

	virtual void on_change_children_order() override
	{
		super_type::on_change_children_order();
		reposition();
	}


	virtual void on_children_size_change(typename super_type::transform_component* _child) override
	{
		super_type::on_children_size_change(_child);
		reposition();
	}


	virtual void on_reposition()
	{
		// アイテムを持っていないのなら何もしない
		if (super_type::items().empty())
		{
			return;
		}

		// 計算したノードの位置を記録する
		struct	posdata
		{
			node_type*	node	= nullptr;
			vec3<float>	pos;
		};
		struct	linedata
		{
			size3<float>			maxSize	= {0, 0, 0};
			std::vector<posdata>	items;
		};
		std::vector<linedata>	positions;

		auto		csize	= this->children_area();

		// 座標を計算する
		vec3<float>	start(0, 0, 0);

		// ライン毎に分割する
		vec3<float>	pos	= start;

		// 1行目を作っておく
		positions.push_back({});
		linedata*	line	= &positions.back();
		for (auto& item : super_type::items())
		{
			auto	itmsz	= item->scaled_size();

			// 右の位置を計算
			auto	rpos	= pos.x + itmsz.width;

			// はみ出す場合は次の行に移行する(ただし一つ目のアイテムなら改行しない)
			if ((mWrap != wrap_type::nowrap) && (rpos > csize.width) && (pos.x > 0))
			{
				// 次のラインへ
				pos.x	=  start.x;
				pos.y	+= (*line).maxSize.height + mSpacing.y;
				rpos	= pos.x + itmsz.width;
				positions.push_back({});
				line	= &positions.back();
			}
			(*line).maxSize.width	= rpos;
			(*line).maxSize.height	= std::max((*line).maxSize.height, itmsz.height);
			(*line).items.push_back({item, pos});
			pos.x	= rpos + mSpacing.x;

		}

		// ライン毎に横方向の調整
		if (mJustify == justify_content_type::end || mJustify == justify_content_type::center)
		{
			// 右揃え
			for (auto& line : positions)
			{
				float	pos	= csize.width - line.maxSize.width;
				if (mJustify == justify_content_type::center)
				{
					pos *= 0.5f;
				}
				for (auto& i : line.items)
				{
					i.pos.x	+= pos;
				}
			}
		}
		else if (mJustify == justify_content_type::space_between || mJustify == justify_content_type::space_around)
		{
			// 左右を埋める形で均等に
			for (auto& line : positions)
			{
				float	w	= csize.width - line.maxSize.width;
				float	s	= mJustify == justify_content_type::space_between
							? w / (float)(line.items.size() - 1)			// space_between
							: w / (float)(line.items.size()    );			// space_around
				float	p	= mJustify == justify_content_type::space_between
							? 0			// space_between
							: s / 2;	// space_around
				for (auto& i : line.items)
				{
					i.pos.x	+= p;
					p		+= s;
				}
			}
		}

		// align items
		if (mAlignSelft == align_self_type::bottom)
		{
			for (auto& line : positions)
			{
				float	height	= line.maxSize.height;
				for (auto& i : line.items)
				{
					i.pos.y	+= height - i.node->size().height;
				}
			}
		}
		else if (mAlignSelft == align_self_type::center)
		{
			for (auto& line : positions)
			{
				float	height	= line.maxSize.height;
				for (auto& i : line.items)
				{
					i.pos.y	+= (height - i.node->size().height) * 0.5f;
				}
			}
		}
		else if (mAlignSelft == align_self_type::stretch)
		{
			for (auto& line : positions)
			{
				float	height	= line.maxSize.height;
				for (auto& i : line.items)
				{
					i.node->set_height(height);
				}
			}
		}

		// 座標を一気に反映させる
		for (auto& line : positions)
		{
			for (auto& i : line.items)
			{
				if (i.node->position() != i.pos)
				{
					mTraits.set_item_position(i.node, i.pos.round(), this);
				}
			}
		}
	}

	align_self_type			mAlignSelft	= align_self_type::top;
	direction_type			mDirection	= direction_type::row;
	justify_content_type	mJustify	= justify_content_type::start;
	wrap_type				mWrap		= wrap_type::wrap;
	vec2<float>				mSpacing	= {0, 0};
	bool					mReposition	= false;
	traits_type				mTraits;
};





template<class SuperT, class... Args>
struct	flexbox_node_traits
		: itemable_node_traits<SuperT, Args...>
{
	template<class N, class This>
	void set_item_position(N* _node, vec3<float> _pos, This* _this)
	{
		_node->set_position(_pos);
	}
};

AMTRS_NAMESPACE_END
#endif
