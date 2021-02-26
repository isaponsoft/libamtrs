/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__circle_node__hpp
#define	__libamtrs__node__circle_node__hpp
#include "node.hpp"
#include "shader/opengles-fill.hpp"
AMTRS_NAMESPACE_BEGIN


// ============================================================================
//! 円ノード
// ----------------------------------------------------------------------------
template<class NodeT>
class	circle_node
		: public NodeT
{
	using	_base_type	= NodeT;
public:
	using	shader_type	= opengles_fill_shader;
	using	vertex		= typename shader_type::vertex;
	using	uniform		= typename shader_type::uniform;

	void set_div_count(std::size_t _count)
	{
		if (mDivCount != _count)
		{
			mDivCount		= _count;
			mVertexModified	= true;
		}
	}

	virtual void on_render(typename _base_type::renderer_argument& _queue) override
	{
		if (mCommandHolder->shader.empty())
		{
			mCommandHolder->shader				= shader_type::create();
			mCommandHolder->primitive			= g3d::primitive_type::triangle_fan;
		}
		if (mVertexModified)
		{
			std::size_t			vtxCount	= mDivCount + 2;
			if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->size() != vtxCount)
			{
				mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(vtxCount);
			}

			mCommandHolder->vertex_buffer->set_translate([this, vtxCount](auto* _mem)
			{
				mVertexModified	= false;
				std::vector<vertex>	vtx(vtxCount);

				auto*	out	= vtx.data();
				auto	sz	= this->size();
				*out++		= {{sz.width/2, sz.height/2, 0, 1}, {1,1,1,1}};
				for (std::size_t c = 0; c <= mDivCount; ++c)
				{
					float	r	= (float)c / (float)mDivCount;
					float	x	= sz.width /2 * std::cos(2.0f * pi * r);
					float	y	= sz.height/2 * std::sin(2.0f * pi * r);
					*out++		= {{sz.width/2 + x, sz.height/2 + y, 0, 1}, {1,1,1,1}};
				}
				_mem->transfer(vtx.data());
			});
		}
		mCommandHolder->uniform.perspective	= _queue.projection;
		mCommandHolder->uniform.coordinate	= this->world_matrix();
		mCommandHolder->uniform.color		= this->get_color();
		_queue.draw(mCommandHolder->make_draw_command());
		_base_type::on_render(_queue);
	}

protected:
	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override
	{
		_base_type::on_size_change(_size);
		mVertexModified	= true;
	}

private:
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	bool										mVertexModified	= true;
	std::size_t									mDivCount		= 16;
};


AMTRS_NAMESPACE_END
#endif
