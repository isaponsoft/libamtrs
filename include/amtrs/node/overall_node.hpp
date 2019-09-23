#ifndef	__libamtrs__node__overall_node__hpp
#define	__libamtrs__node__overall_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN


// ****************************************************************************
//! 全体を覆うように自身のサイズを調整します。
// ----------------------------------------------------------------------------
//! ルートノード全体を覆い隠すように自身の位置をサイズを自動調整します。
//!
//!	example)
//!		画面全体を黒い半透明で覆います。
//!		ただし、描画の優先順位には気を付けてください。
//!
//! 	auto*	modal = input_untransparentable_node<overall_node<fill_node<node>>>
//!		modal->set_color({0, 0, 0, 0.5f});
//!		this->add_child(modal);
// ----------------------------------------------------------------------------
template<class NodeT>
class	overall_node
		: public updatable_node<NodeT>
{
	using	_base_type		= updatable_node<NodeT>;
public:
	using	_base_type::_base_type;

protected:
	virtual void on_created() override
	{
		_base_type::on_created();

		auto*	top	= _base_type::top();
		_base_type::set_size(top->size());
		_base_type::set_position(- _base_type::world_matrix().translate());
	}
};


AMTRS_NAMESPACE_END
#endif
