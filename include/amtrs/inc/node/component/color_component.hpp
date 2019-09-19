/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__node__component__color_component__hpp
#define	__libamtrs__node__component__color_component__hpp
#include "../../../graphics.hpp"
AMTRS_NAMESPACE_BEGIN namespace component {

template<class Node>
class	color_component
{
public:
	using	color_type		= rgba<float>;

	const color_type& get_color() const noexcept
	{
		return	mColor;
	}

	void set_alpha(float _alpha) noexcept
	{
		if (mColor.a != _alpha)
		{
			mColor.a	= _alpha;
			on_chnage_color();
		}
	}

	void set_color(color_type _color) noexcept
	{
		if (mColor != _color)
		{
			mColor	= _color;
			on_chnage_color();
		}
	}

protected:
	virtual void on_chnage_color() noexcept
	{}

private:
	color_type				mColor		= {1, 1, 1, 1};
};


} AMTRS_NAMESPACE_END
#endif
