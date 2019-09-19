#ifndef	__libamtrs__node__vertical_pager_node__hpp
#define	__libamtrs__node__vertical_pager_node__hpp
#include "flickable_pager_node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class NodeT>
class	vertical_pager_node
		: public flickable_pager_node<NodeT>
{
	using	_base_type	= flickable_pager_node<NodeT>;
public:

	vertical_pager_node()
		: _base_type(uiutils::scroll::axis_y)
	{}

protected:
	virtual vec3<float> compute_page_position(typename _base_type::index_type _page) const override
	{
		return	{ 0, this->size().height * float(_page), 0 };
	}
};


AMTRS_NAMESPACE_END
#endif
