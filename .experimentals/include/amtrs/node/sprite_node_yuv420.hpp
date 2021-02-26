/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__sprite_node_yuv420__hpp
#define	__libamtrs__node__sprite_node_yuv420__hpp
#include "node.hpp"
#include "shader/opengles-sprite-yuv420.hpp"
AMTRS_NAMESPACE_BEGIN


template<class NodeT>
class	sprite_node_yuv420
		: public NodeT
{
public:
	using	super_type		= NodeT;
	using	shader_type		= opengles_sprite_yuv420_shader;
	using	texture_type	= g3d::texture;
	using	vertex			= shader_type::vertex;
	using	uniform			= shader_type::uniform;

	sprite_node_yuv420()
	{}

	template<class... Args>
	sprite_node_yuv420(Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
	{
	}


	template<class... Args>
	sprite_node_yuv420(ref<texture_type> _texture, Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
	{
		set_texture(std::move(_texture));
	}


	vec2<float> uv_scale() const noexcept { return mUVScale; }
	void set_uv_scale(vec2<float> _scale) noexcept { mUVScale = _scale; mUpdateVertex	= true; }

	void set_draw_status(const g3d::draw_status& _ds) noexcept { mCommandHolder->status = _ds; }
	g3d::draw_status get_draw_status() const noexcept { return mCommandHolder->status; }

	void set_texture(ref<g3d::texture> _textY, ref<g3d::texture> _textU, ref<g3d::texture> _textV, bool _update_size = true)
	{
		if (mCommandHolder->uniform.texture_y != _textY)
		{
			super_type::rerender_request();
			mCommandHolder->uniform.texture_y	= std::move(_textY);
			mCommandHolder->uniform.texture_u	= std::move(_textU);
			mCommandHolder->uniform.texture_v	= std::move(_textV);
			mUpdateVertex						= true;
			if (!mCommandHolder->uniform.texture_y.empty() && _update_size)
			{
				auto	sz	= typename super_type::size_type(mCommandHolder->uniform.texture_y->size());
				this->set_size(sz);
			}
		}
	}


	virtual void on_render(typename super_type::renderer_argument& _queue) override
	{
		// シェーダーと頂点バッファを生成しておく
		if (mCommandHolder->shader.empty())
		{
			mCommandHolder->status.set_alpha_blend(g3d::draw_status::blend_type::src_alpha, g3d::draw_status::blend_type::invrs_src_alpha);
			mCommandHolder->status.set_texture2d_filter
			(
				g3d::draw_status::texture2d_filter_type::liner,
				g3d::draw_status::texture2d_filter_type::liner
			);
			mCommandHolder->shader			= shader_type::create();
			mCommandHolder->primitive		= g3d::primitive_type::triangle_strip;
		}


		// 頂点情報の更新が必要
		if (mUpdateVertex)
		{
			mUpdateVertex	= false;

			// 描画処理中に頂点のデータを書き換えるとよろしくないので
			// 頂点情報が変わったら頂点バッファを生成しなおす
			mCommandHolder->vertex_buffer		= new g3d::vertex_buffer<vertex>(4);
			mCommandHolder->vertex_buffer->set_translate([uvs = mUVScale, tex = mCommandHolder->uniform.texture_y, sz = vec2<float>(sprite_size())](auto _mem)
			{
				auto	pos1	= vec2<float>(0, 0);
				auto	pos2	= pos1 + sz;
				vertex	vtx[4] = 
				{
					{{pos1.x, pos1.y, 0, 1}, {0    , 0    }, {1,1,1,1} },
					{{pos2.x, pos1.y, 0, 1}, {uvs.x, 0    }, {1,1,1,1} },
					{{pos1.x, pos2.y, 0, 1}, {0    , uvs.y}, {1,1,1,1} },
					{{pos2.x, pos2.y, 0, 1}, {uvs.x, uvs.y}, {1,1,1,1} },
				};
				auto	tc	= tex->coordinate();
				for (auto& v : vtx)
				{
					v.uv *= tc;
				}
				_mem->transfer(vtx);
			});
		}

		if (mCommandHolder->uniform.texture_y && mCommandHolder->vertex_buffer)
		{
			mCommandHolder->uniform.color		= this->get_color();
			mCommandHolder->uniform.perspective	= _queue.projection;
			mCommandHolder->uniform.coordinate	= this->world_matrix();
			_queue.draw(mCommandHolder->make_draw_command());
		}
		super_type::on_render(_queue);
	}

	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		super_type::rerender_request();
		mUpdateVertex		= true;
	}

	virtual void on_change_render_status() override
	{
		super_type::on_change_render_status();
		super_type::rerender_request();
		mUpdateVertex		= true;
	}

protected:
	virtual size2<float> sprite_size() const
	{
		return	{ this->size().width, this->size().height };
	}

	bool										mUpdateVertex	= false;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder	= new g3d::draw_command_holder<shader_type>();
	vec2<float>									mUVScale		= {1, 1};
};

AMTRS_NAMESPACE_END
#endif
