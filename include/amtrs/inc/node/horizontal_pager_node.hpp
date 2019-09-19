#ifndef	__libamtrs__node__horizontal_pager_node__hpp
#define	__libamtrs__node__horizontal_pager_node__hpp
#include "flickable_pager_node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class NodeT>
class	horizontal_pager_node
		: public flickable_pager_node<NodeT>
{
	using	_base_type	= flickable_pager_node<NodeT>;
public:

	horizontal_pager_node()
		: _base_type(uiutils::scroll::axis_x)
	{}

protected:
	virtual vec3<float> compute_page_position(typename _base_type::index_type _page) const override
	{
		return	{ this->size().width * float(_page), 0, 0 };
	}
};


AMTRS_NAMESPACE_END
#endif
