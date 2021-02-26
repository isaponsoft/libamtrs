/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__window_frame_node__hpp
#define	__libamtrs__node__window_frame_node__hpp
#include <map>
#include "node.hpp"
#include "children_transform_node.hpp"
#include "shader/opengles-sprite.hpp"
AMTRS_NAMESPACE_BEGIN



// ****************************************************************************
//! ウィンドウフレームを表示します。
// ----------------------------------------------------------------------------
//! Android の 9 ptach に似ていますが、拡大の代わりにリピートを行います。
//! 
// ----------------------------------------------------------------------------
template<class NodeT>
class	window_frame_node
		: public children_transform_node<NodeT>
{
	using	_base_type	= children_transform_node<NodeT>;
public:
	using	size_type	= typename _base_type::size_type;
	using	shader_type	= opengles_sprite_shader;
	using	vertex_type	= typename shader_type::vertex;

	using	_base_type::_base_type;

	// ========================================================================
	//! 何もせず初期化します。
	// ------------------------------------------------------------------------
	window_frame_node() {}

	// ========================================================================
	//! テクスチャを指定して初期化します。
	// ------------------------------------------------------------------------
	//! 反復部分は position = (25%, 25%), size = (50%, 50%) になります。
	// ------------------------------------------------------------------------
	window_frame_node(g3d::texture* _texture) { set_texture(_texture); }


	// ========================================================================
	//! テクスチャを指定して初期化します。
	// ------------------------------------------------------------------------
	window_frame_node(g3d::texture* _texture, rect<int> _repeatRect) { set_texture(_texture, _repeatRect); }

	// ========================================================================
	//! テクスチャを設定します。
	// ------------------------------------------------------------------------
	//! 反復部分は position = (25%, 25%), size = (50%, 50%) になります。
	// ------------------------------------------------------------------------
	void set_texture(g3d::texture* _texture);


	// ========================================================================
	//! テクスチャを設定します。
	// ------------------------------------------------------------------------
	//! _repeatRect
	//!		反復表示を行う部分のテクスチャの矩形を指定します。
	// ------------------------------------------------------------------------
	void set_texture(g3d::texture* _texture, rect<int> _repeatRect);


	// ========================================================================
	//! テクスチャを取得します。
	// ------------------------------------------------------------------------
	g3d::texture* texture() const noexcept { return mCommandHolder->uniform.texture; }

	// ========================================================================
	//! コンテンツ領域の大きさを取得します。
	// ------------------------------------------------------------------------
	size_type content_size() const noexcept { return mContentSize; }

	// ========================================================================
	//! テクスチャサイズに合わせてアライメントされたノードの大きさを計算します。
	// ------------------------------------------------------------------------
	//! 事前にテクスチャを設定しておく必要があります。
	// ------------------------------------------------------------------------
	size_type align_size(size_type _size, bool _minimal) const noexcept
	{
		size_type	retval	= _size;
		compute_parts_size();
		auto		rep	= repeat_count(retval, _minimal);
		retval.width	= mFrameML.width  + mFrameMC.width  * rep.x + mFrameMR.width;
		retval.height	= mFrameTC.height + mFrameMC.height * rep.y + mFrameBC.height;
		return	retval;
	}


	// ========================================================================
	//! 指定するコンテンツ領域が収まるノードの大きさを計算します。
	// ------------------------------------------------------------------------
	//! 事前にテクスチャを設定しておく必要があります。
	// ------------------------------------------------------------------------
	void set_size_by_aligned(size_type _size)
	{
		this->set_size(align_size(_size, true));
	}

protected:
	virtual void on_size_change(typename _base_type::size_type& _size) noexcept override;

	virtual void on_render(typename _base_type::render_queue& _queue) override
	{
		if (mUpdateDirty)
		{
			mUpdateDirty		= false;
			compute_parts_size();

			// レンダリングパラメータの設定
			mCommandHolder->status.set_alpha_blend(g3d::draw_status::blend_type::src_alpha, g3d::draw_status::blend_type::invrs_src_alpha);
			mCommandHolder->status.set_texture2d_filter
			(
				g3d::draw_status::texture2d_filter_type::point,
				g3d::draw_status::texture2d_filter_type::point
			);

			auto			rep						= repeat_count(this->size(), true);
			if (rep.x <= 0 || rep.y <= 0)
			{
				_base_type::on_render(_queue);
				return;
			}

			// 頂点バッファを必要に応じて再構築
			std::size_t		vertexCount				= (rep.x + 2) * (rep.y + 2) * 6;
			if ((int)vertexCount < 0)
			{
				_base_type::on_render(_queue);
				return;
			}
			if (mCommandHolder->vertex_buffer.empty() || mCommandHolder->vertex_buffer->size() != vertexCount)
			{
				mCommandHolder->vertex_buffer	= new g3d::vertex_buffer<vertex_type>(vertexCount);
			}

			std::vector<vertex_type>	buff(vertexCount);
			mCommandHolder->primitive		= g3d::primitive_type::triangles;

			// 内側の作成
			auto		pos1	= vec3<float>(0, 0, 0);
			auto*		vtx		= buff.data();

			// １行目
			vtx = make_vertex(vtx, pos1, mFrameTL);
			for (int x = rep.x; x  > 0; --x)
			{
				vtx = make_vertex(vtx, pos1, mFrameTC);
			}
			vtx = make_vertex(vtx, pos1, mFrameTR);

			// ２行目～
			pos1.x	=  0;
			pos1.y	+= mFrameTC.height;
			for (int y = rep.y; y > 0; --y)
			{
				vtx = make_vertex(vtx, pos1, mFrameML);
				for (int x = 0; x < rep.x; ++x)
				{
					vtx = make_vertex(vtx, pos1, mFrameMC);
				}
				vtx = make_vertex(vtx, pos1, mFrameMR);

				pos1.x	=  0;
				pos1.y	+= mFrameMC.height;
			}

			// 一番下
			vtx = make_vertex(vtx, pos1, mFrameBL);
			for (int x = rep.x; x  > 0; --x)
			{
				vtx = make_vertex(vtx, pos1, mFrameBC);
			}
			vtx = make_vertex(vtx, pos1, mFrameBR);

			auto	tc	= this->texture()->coordinate();
			for (auto& v : buff)
			{
				v.uv *= tc;
			}

			mCommandHolder->vertex_buffer->set_translate([v = std::move(buff)](auto* _mem)
			{
				_mem->transfer(v.data());
			});
		}

		mCommandHolder->uniform.color		= this->get_color();
		mCommandHolder->uniform.perspective	= _queue.projection;
		mCommandHolder->uniform.coordinate	= this->render_matrix();
		if (mCommandHolder->shader.empty())
		{
			mCommandHolder->shader	= shader_type::create();
		}
		_queue.draw(mCommandHolder->make_draw_command());
		_base_type::on_render(_queue);
	}

private:

	// パーツの大きさを計算する。
	void compute_parts_size() const noexcept;

	vec2<int> repeat_count(size_type _size, bool _minimal) const noexcept
	{
		size2<int>	apd	= _minimal
						? size2<int>(0, 0)
						: size2<int>((int)mFrameMC.width - 1, (int)mFrameMC.height - 1);
		return	{
			std::max<int>(1, ((int)(_size.width  - mFrameML.width  - mFrameMR.width ) + apd.width ) / (int)mFrameMC.width ),
			std::max<int>(1, ((int)(_size.height - mFrameTC.height - mFrameBC.height) + apd.height) / (int)mFrameMC.height)
		};
	}

	template<class T>
	T* make_vertex(T* _vtx, vec3<float>& _pos1, rect<float> _parts)
	{
		auto	_pos2	= _pos1 + vec3<float>(vec2<float>(_parts.size()), 0);
		auto	_uv1	= vec2<float>(_parts.position()) / vec2<float>(size2<float>(mCommandHolder->uniform.texture->size()));
		auto	_uv2	= _uv1  + vec2<float>(_parts.size()) / vec2<float>(size2<float>(mCommandHolder->uniform.texture->size()));
		*_vtx++	= vertex_type{{_pos1.x, _pos1.y, 0, 1}, {_uv1.x, _uv1.y}, {1,1,1,1} };
		*_vtx++	= vertex_type{{_pos2.x, _pos1.y, 0, 1}, {_uv2.x, _uv1.y}, {1,1,1,1} };
		*_vtx++	= vertex_type{{_pos1.x, _pos2.y, 0, 1}, {_uv1.x, _uv2.y}, {1,1,1,1} };
		*_vtx++	= vertex_type{{_pos2.x, _pos1.y, 0, 1}, {_uv2.x, _uv1.y}, {1,1,1,1} };
		*_vtx++	= vertex_type{{_pos2.x, _pos2.y, 0, 1}, {_uv2.x, _uv2.y}, {1,1,1,1} };
		*_vtx++	= vertex_type{{_pos1.x, _pos2.y, 0, 1}, {_uv1.x, _uv2.y}, {1,1,1,1} };
		_pos1.x	= _pos2.x;
		return	_vtx;
	}

	mutable bool								mUpdateDirty		= false;
	ref<g3d::draw_command_holder<shader_type>>	mCommandHolder		= new g3d::draw_command_holder<shader_type>();

	// パーツ分割時の大きさ。テクスチャ設定時に調整されます。
	rect<float>									mRepeatRect;
	mutable rect<float>							mFrameTL;
	mutable rect<float>							mFrameTC;
	mutable rect<float>							mFrameTR;
	mutable rect<float>							mFrameML;
	mutable rect<float>							mFrameMC;
	mutable rect<float>							mFrameMR;
	mutable rect<float>							mFrameBL;
	mutable rect<float>							mFrameBC;
	mutable rect<float>							mFrameBR;
	mutable size_type							mContentSize		= {0, 0, 0};
};


template<class NodeT>
void window_frame_node<NodeT>::set_texture(g3d::texture* _texture)
{
	auto		ts	= size2<int>(_texture->size());
	rect<int>	repeat{
		ts.width/4,
		ts.height/4,
		ts.width/2,
		ts.height/2,
	};
	set_texture(_texture, repeat);
}


template<class NodeT>
void window_frame_node<NodeT>::compute_parts_size() const noexcept
{
	if (mCommandHolder->uniform.texture)
	{
		size2<float>	ts	= size2<float>(mCommandHolder->uniform.texture->size());

		// 横方向のテクスチャ位置とスプライトサイズを決定
		mFrameTL.x			= mFrameML.x		= mFrameBL.x		= 0;
		mFrameTC.x			= mFrameMC.x		= mFrameBC.x		= mRepeatRect.left();
		mFrameTR.x			= mFrameMR.x		= mFrameBR.x		= mRepeatRect.right();
		mFrameTL.width		= mFrameML.width	= mFrameBL.width	= mRepeatRect.left();
		mFrameTC.width		= mFrameMC.width	= mFrameBC.width	= mRepeatRect.width;
		mFrameTR.width		= mFrameMR.width	= mFrameBR.width	= ts.width - mRepeatRect.right();

		// 縦方向のテクスチャ位置とスプライトサイズを決定
		mFrameTL.y			= mFrameTC.y		= mFrameTR.y		= 0;
		mFrameML.y			= mFrameMC.y		= mFrameMR.y		= mRepeatRect.top();
		mFrameBL.y			= mFrameBC.y		= mFrameBR.y		= mRepeatRect.bottom();
		mFrameTL.height		= mFrameTC.height	= mFrameTR.height	= mRepeatRect.top();
		mFrameML.height		= mFrameMC.height	= mFrameMR.height	= mRepeatRect.height;
		mFrameBL.height		= mFrameBC.height	= mFrameBR.height	= ts.height - mRepeatRect.bottom();


		auto			rep	= repeat_count(this->size(), true);
		mContentSize		=
		{
			mFrameMC.width  * rep.x,
			mFrameMC.height * rep.y,
			0,
		};
	}
}


template<class NodeT>
void window_frame_node<NodeT>::set_texture(g3d::texture* _texture, rect<int> _repeatRect)
{
	mCommandHolder->uniform.texture	= _texture;
	mRepeatRect				= rect<float>(_repeatRect);
	compute_parts_size();
	mUpdateDirty			= true;
}


template<class NodeT>
void window_frame_node<NodeT>::on_size_change(typename _base_type::size_type& _size) noexcept
{
	_base_type::on_size_change(_size);
	compute_parts_size();
	_base_type::set_children_size(content_size());
	_base_type::set_children_position({mFrameML.width, mFrameTC.height, 0});
	mUpdateDirty			= true;
}



AMTRS_NAMESPACE_END
#endif
