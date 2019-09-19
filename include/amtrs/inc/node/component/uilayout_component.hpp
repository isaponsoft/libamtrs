/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__node__component__uilayout_component__hpp
#define	__libamtrs__node__component__uilayout_component__hpp
AMTRS_NAMESPACE_BEGIN namespace component {


template<class NodeT>
class	uilayout_component
{
public:
	struct	uilayout_data
	{
		//! フォントの大きさ
		float	font_size;

		//! テキストに対するマージン
		float	text_margin;

		//! アイコンに対するマージン
		float	icon_margin;

		//! UIノードの基本となる縦幅
		float	node_height;

		//! アイコンの基本となる縦幅
		float	icon_height;
	};

	uilayout_data uilayout() const
	{
		uilayout_data	retval;
		retval.font_size		= static_cast<float>(static_cast<const NodeT*>(this)->get_director()->font->size());
		retval.text_margin	= retval.font_size * 0.5f;
		retval.icon_margin	= retval.font_size * 0.3f;
		retval.node_height	= retval.font_size + retval.text_margin * 2;
		retval.icon_height	= retval.node_height - retval.icon_margin * 2;
		return	retval;
	}
};



} AMTRS_NAMESPACE_END
#endif
