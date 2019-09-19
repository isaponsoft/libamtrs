#ifndef	__libamtrs__node__scroll_node__hpp
#define	__libamtrs__node__scroll_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN
using namespace uiutils;


// ============================================================================
//! ノードに対してスクロール機能を持たせます。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
template<class NodeT>
class	scroll_node
		: public updatable_node<NodeT>
{
	using	_base_type		= updatable_node<NodeT>;
public:

	using	matrix_type		= typename NodeT::matrix_type;
	using	position_type	= typename NodeT::position_type;

	static constexpr auto	axis_x	= scroll::axis_x;
	static constexpr auto	axis_y	= scroll::axis_y;


	scroll_node(uint32_t _scroll_axis = scroll::axis_y)
		: mScroller(_scroll_axis)
	{}


	void set_multi_scroll(bool _enable)
	{
		mScroller.set_multi_scroll(_enable);
	}


	vec3<float> get_pinch_scale() const noexcept
	{
		return	mScrollMatrix.scale();
	}

	void set_scroll_enable(bool _enable) noexcept
	{
		mEnableScroll	= _enable;
	}

	virtual void on_input_cancel() override
	{
		_base_type::on_input_cancel();
		mScroller.cancel(*this);
	}


	virtual typename NodeT::node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		auto	mat		= NodeT::world_matrix().inverse();
		auto	mouse	= _e->event()->get_mouse_event();
		if (mouse && mEnableScroll)
		{
			if (mScroller.is_capturing())
			{
				auto	pos	= vec2<float>(vec3<float>(mouse->template get_point<float>(),0) * mat);
				if (mouse->type == input_event::mouse)
				{
					mScroller.input(*this, mouse, &pos);
					on_coordinate_modify();
				}
				return	this;
			}

			if (auto r = _base_type::on_input(_e); r)
			{
				mScroller.release();
				return	r;
			}

			auto	pos	= vec2<float>(vec3<float>(mouse->template get_point<float>(),0) * mat);
			if (mouse->type == input_event::mouse)
			{
				mScroller.input(*this, mouse, &pos);
				this->on_coordinate_modify();
			}

			return	mScroller.is_capturing() ? this : nullptr;
		}
		else
		{
			return	_base_type::on_input(_e);
		}
	}


	virtual void on_update(float _delta) override
	{
		_base_type::on_update(_delta);
		mScroller.update(_delta);

		auto	now	= vec3<float>(mScroller.position());
		if (mScrollPrev != now)
		{
			vec2<float>	scroll(now.x, now.y);
			on_scrolling(scroll);
			if ((scroll.x != mScrollPrev.x) || (scroll.y != mScrollPrev.y))
			{
				mScrollMatrix *= mat44<float>::translate(now - mScrollPrev);
				mScrollPrev = now;
				on_coordinate_modify();
			}
		}
	}

	virtual const matrix_type& world_matrix() const noexcept override
	{
		if (mModifiedMatrix)
		{
			mModifiedMatrix	= false;
			mMatrix = _base_type::world_matrix()
					* mScrollMatrix
			;
		}
		return	mMatrix;
	}

	vec2<float> get_scroll() const noexcept { return vec2<float>(mScroller.position()); }

	void set_scroll_range_x(range<float> _range)
	{
		if (mScroller.range_x() != _range)
		{
			mScroller.set_range_x(_range);
			on_coordinate_modify();
		}
	}

	void set_scroll_range_y(range<float> _range)
	{
		if (mScroller.range_y() != _range)
		{
			mScroller.set_range_y(_range);
			on_coordinate_modify();
		}
	}

	range<float> scroll_range_x() const noexcept { return mScroller.range_x(); }
	range<float> scroll_range_y() const noexcept { return mScroller.range_y(); }

protected:
	// ========================================================================
	//! スクロール量が変化したときに呼ばれる。
	// ------------------------------------------------------------------------
	//! 引数の値を変更するとスクロール量を変更できます。
	// ------------------------------------------------------------------------
	virtual void on_scrolling(vec2<float>& _scroll) noexcept { }


	virtual void on_coordinate_modify() noexcept override
	{
		mModifiedMatrix	= true;
		_base_type::on_coordinate_modify();
	}



	mat44<float>		mScrollMatrix			= mat44<float>::identify();
	vec3<float>			mScrollPrev				= {0, 0, 0};
	scroll				mScroller;
	mutable bool		mModifiedMatrix			= true;
	mutable matrix_type	mMatrix;
	bool				mEnableScroll			= true;
};


AMTRS_NAMESPACE_END
#endif
