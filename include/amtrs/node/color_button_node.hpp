#ifndef	__libamtrs__node__color_button_node__hpp
#define	__libamtrs__node__color_button_node__hpp
#include "node.hpp"
#include "fill_node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class NodeT>
class	color_button_node
		: public button_node<fill_node<NodeT>>
{
	using	base_type	= button_node<fill_node<NodeT>>;

public:
	color_button_node(rgba<float> _released, rgba<float> _pressed)
		: mReleasedColor(_released)
		, mPressedColor(_pressed)
	{
		base_type::set_color(mReleasedColor);
	}

	void set_released_color(rgba<float> _color)
	{
		mReleasedColor	= _color;
	}

	void set_pressed_color(rgba<float> _color)
	{
		mPressedColor	= _color;
	}

protected:
	// ========================================================================
	//! ボタンの押下状態が変化したときに呼ばれる。
	// ------------------------------------------------------------------------
	//! 表示するグラフィックの変更のみを行うこと。
	// ------------------------------------------------------------------------
	virtual void on_change_press_state(bool _press) override
	{
		base_type::set_color
		(
			_press
			? mPressedColor
			: mReleasedColor
		);
	}

	rgba<float>	mReleasedColor;
	rgba<float>	mPressedColor;
};


AMTRS_NAMESPACE_END
#endif
