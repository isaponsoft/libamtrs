#ifndef	__libamtrs__node__camera_node__hpp
#define	__libamtrs__node__camera_node__hpp
#include "node.hpp"
AMTRS_NAMESPACE_BEGIN

/*
class	camera_node
		: public node
{
public:
	vec3<float>	mCameraPos	= {0, 0, 0};

	virtual void on_render(typename node::render_queue& _rs) override
	{
		typename node::render_queue	rs	= _rs;
		
		rs.projection	= mat44<float>::translate(-mCameraPos) * rs.projection;

		node::on_render(rs);
	}
};
*/


AMTRS_NAMESPACE_END
#endif
