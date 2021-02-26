/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__viewport_node__hpp
#define	__libamtrs__node__viewport_node__hpp
#include "../g3d.hpp"
#include "../node.hpp"
AMTRS_NAMESPACE_BEGIN



template<class BaseT>
class	viewport_node
		: public BaseT
{
public:
	using	super_type		= BaseT;
	using	renderer_type	= typename super_type::renderer_type;
	using	super_type::super_type;


protected:
	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
	}

	virtual void on_render(renderer_type& _queue) override
	{
		mat44<float>	prev	= _queue.projection;
		_queue.projection	= mat44<float>::ortho(vec2<float>(size2<float>(this->size()))/2, size2<float>(this->size())); 
		super_type::on_render(_queue);
		_queue.projection	= prev;
	}
};

AMTRS_NAMESPACE_END
#endif
