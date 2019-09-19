#ifndef	__libamtrs__node__input_untransparentable_node__hpp
#define	__libamtrs__node__input_untransparentable_node__hpp
#include "scroll_node.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 入力を非透過型にします。
// ----------------------------------------------------------------------------
//! このテンプレートを使うと自身より奥に配置されているノードはポインタ入力を
//! 受け取らなくなります。
// ----------------------------------------------------------------------------
template<class NodeT>
class	input_untransparentable_node
		: public NodeT
{
	using	_base_type	= NodeT;
public:
	using	_base_type::_base_type;

	enum class input_intrruput_type
	{
		self_area,						//!< 自身と同じ範囲のみ入力を割り込む
		all_area,						//!< 自身のエリアに関係なく入力を割り込む
	};

	static constexpr input_intrruput_type	self_area	= input_intrruput_type::self_area;
	static constexpr input_intrruput_type	all_area	= input_intrruput_type::all_area;


	void set_input_intrrupt_mode(input_intrruput_type _type) noexcept
	{
		mMode	= _type;
	}

protected:
	virtual typename _base_type::node_type* on_input(typename _base_type::pointer_event* _e) override
	{
		if (auto r = _base_type::on_input(_e); r)
		{
			return	r;
		}

		if (mMode == self_area)
		{
			if (auto mouse = _e->event()->get_mouse_event(); mouse)
			{
				auto	mat		= _base_type::render_matrix().inverse();
				if (_base_type::size().is_inner(mouse->template get_point<float>() * mat))
				{
					return	_base_type::input_intercept();
				}
			}
			if (auto wheel = _e->event()->get_mouse_wheel_event(); wheel)
			{
				auto	mat		= _base_type::render_matrix().inverse();
				if (_base_type::size().is_inner(wheel->template get_point<float>() * mat))
				{
					return	_base_type::input_intercept();
				}
			}
		}
		else
		{
			if (_e->event()->get_mouse_event() || _e->event()->get_mouse_wheel_event())
			{
				return	_base_type::input_intercept();
			}
		}
		return	nullptr;
	}

private:
	input_intrruput_type		mMode	= self_area;
};


AMTRS_NAMESPACE_END
#endif
