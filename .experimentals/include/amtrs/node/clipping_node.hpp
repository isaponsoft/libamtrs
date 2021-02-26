/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__clipping_node__hpp
#define	__libamtrs__node__clipping_node__hpp
#include "../graphics/rect.hpp"
#include "fill_node.hpp"
#include "shader/opengles-fill.hpp"
#include AMTRS_DRIVER_INCLUDE(api/opengl.hpp)
AMTRS_NAMESPACE_BEGIN
template<class SuperT, class AreaT>	class	basic_clipping_node;
struct	clipping_traits;
struct	children_clipping_traits;
struct	clipping_with_parent_traits;



// ****************************************************************************
//! SuperTと子ノードの描画領域とポインタ入力範囲をクリッピングします。
// ----------------------------------------------------------------------------
//! 自身のサイズに合わせてクリッピングします。
// ----------------------------------------------------------------------------
template<class SuperT>
using	clipping_node			= basic_clipping_node<SuperT, clipping_traits>;


// ****************************************************************************
//! SuperTと子ノードの描画領域とポインタ入力範囲をクリッピングします。
// ----------------------------------------------------------------------------
//! 自身の子領域のサイズに合わせてクリッピングします。
// ----------------------------------------------------------------------------
template<class SuperT>
using	children_clipping_node	= basic_clipping_node<SuperT, children_clipping_traits>;


#if	0
// ****************************************************************************
//! SuperTと子ノードの描画領域とポインタ入力範囲をクリッピングします。
// ----------------------------------------------------------------------------
//! 親のサイズに併せてクリッピングします。
// ----------------------------------------------------------------------------
template<class SuperT>
using	parent_clipping_node	= basic_clipping_node<SuperT, clipping_with_parent_traits>;
#endif


// ****************************************************************************
//! クリッピングマスクを作成します。
// ----------------------------------------------------------------------------
//! 
// ----------------------------------------------------------------------------
template<class SuperT, class AreaT>
class	basic_clipping_node
		: public SuperT
{
public:
	using	super_type		= SuperT;
	using	node_type		= typename super_type::node_type;
	using	shader_type		= opengles_fill_shader;
	using	vertex			= shader_type::vertex;
	using	uniform			= shader_type::uniform;
	using	area_type		= AreaT;

	using	super_type::super_type;

	void set_stencil_id(uint8_t _value)
	{
		mStencilCode	= _value;
	}

protected:

	virtual node_type* on_input(typename super_type::pointer_event* _e) override
	{
		//! 自分の範囲外のプレスイベントは子ノードに伝えないようにする。
		if (auto mouse = _e->event()->get_mouse_event(); mouse && mouse->state == input_event::press)
		{
			auto	mat		= super_type::world_matrix().inverse();
			if (!area_type().bounds(this).is_inner(mouse->template get_point<float>() * mat))
			{
				return	nullptr;
			}
		}
		return	super_type::on_input(_e);
	}

	virtual void on_render(typename super_type::renderer_argument& _queue) override
	{
		// ステンシルバッファをクリアし
		// ステンシルバッファに 1 を書き込むように指定する。
		_queue.draw([this](auto g)
		{
			glGetIntegerv(GL_ALPHA_TEST_FUNC, &mAlphaFunc);
			glGetFloatv(GL_ALPHA_TEST_REF, &mAlphaRef);

			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);

			glClearStencil(0);
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, mStencilCode, ~0);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		//	glAlphaFunc(GL_ALWAYS, (float)0);
			glColorMask(0, 0, 0, 0);
			glDepthMask(0);
		});
		draw(_queue);

		// 子ノードに対してステンシルテストを行う
		_queue.draw([this](auto g)
		{
		//	glAlphaFunc(mAlphaFunc, mAlphaRef);
			glStencilFunc(GL_EQUAL, mStencilCode, ~0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glEnable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glColorMask(1,1,1,1);
			glDepthMask(1);
		});
		super_type::on_render(_queue);

		// ステンシルテストを終了する
		_queue.draw([](auto g)
		{
			glDisable(GL_STENCIL_TEST);
		});
	}

	void update_vertex(g3d::device* _device)
	{
		if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->count() != 4)
		{
			mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(4);
		}

		mCommandHolder->vertex_buffer->set_translate([bounds = area_type().bounds(this)](auto* _mem)
		{
			auto	pos1	= vec2<float>(bounds.position());
			auto	pos2	= pos1 + vec2<float>(size2<float>(bounds.size()));
			vertex	vtx[4] = 
			{
				{{pos1.x, pos1.y, 0, 1}, {0,0,0,1} },
				{{pos2.x, pos1.y, 0, 1}, {0,0,0,1} },
				{{pos1.x, pos2.y, 0, 1}, {0,0,0,1} },
				{{pos2.x, pos2.y, 0, 1}, {0,0,0,1} },
			};
			_mem->transfer(vtx);
		});
	}

	void draw(typename super_type::renderer_argument& _queue)
	{
		auto*	device	= _queue.device;
		mCommandHolder->uniform.perspective	= _queue.projection;
		mCommandHolder->uniform.coordinate	= this->world_matrix();
		mCommandHolder->uniform.color		= {0,0,0,1};
		mCommandHolder->status.alpha_blend	= false;
		if (mCommandHolder->shader.empty())
		{
			mCommandHolder->shader				= shader_type::create();
			mCommandHolder->primitive			= g3d::primitive_type::triangle_strip;
		}
		if (mVertexModified)
		{
			mVertexModified	= false;
			update_vertex(device);
		}

		_queue.draw(mCommandHolder->make_draw_command());
	}

	void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		mVertexModified	= true;
	}

	uint8_t										mStencilCode	= 1;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	bool										mVertexModified	= true;
	GLint 										mAlphaFunc;
	GLfloat										mAlphaRef;
};


struct	clipping_traits
{
	template<class Node>
	rect<float> bounds(Node* _node) const noexcept
	{
		return	rect<float>({0,0}, size2<float>(_node->size()));
	}
};


struct	children_clipping_traits
{
	template<class Node>
	rect<float> bounds(Node* _node) const noexcept
	{
		auto	b	= _node->children_area();
		return	rect<float>(vec2<float>(b.position()), size2<float>(b.size()));
	}
};


#if	0
struct	clipping_with_parent_traits
{
	template<class Node>
	rect<float> bounds(Node* _node) const noexcept
	{
		if (_node->parent())
		{
			return	rect<float>({0,0}, _node->parent()->children_area());
		}
		else
		{
			return	rect<float>({0,0}, {0,0});
		}
	}
};
#endif


AMTRS_NAMESPACE_END
#endif
