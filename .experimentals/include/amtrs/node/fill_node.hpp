/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__fill_node__hpp
#define	__libamtrs__node__fill_node__hpp
#include "../g3d.hpp"
#include "../node.hpp"
AMTRS_NAMESPACE_BEGIN

template<class BaseT>
class	fill_node
		: public BaseT
{
public:
	using	super_type		= BaseT;
	using	renderer_type	= typename super_type::renderer_type;
	using	shader_type		= g3d::basicshader::fillshader;
	using	uniform_type	= typename shader_type::uniform;
	using	vertex_type		= typename shader_type::vertex;
	using	super_type::super_type;

protected:
	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		mVertics	= {};
	}

	virtual void on_render(renderer_type& _queue) override
	{
		if (mMaterial.empty())
		{
			mMaterial	= shader_type::create(_queue.transfer(), g3d::device::api_type::vulkan);
			mConstBuff	= g3d::constants(mMaterial, 0);
			mPipeline	= g3d::pipeline(mMaterial, {});
		}

		if (mVertics.empty())
		{
			const vertex_type vertexData[] =
			{
				{ .position = {-1.0f, -1.0f, 0.0f, 1 }, .color = {1,1,1,1} },
				{ .position = { 1.0f, -1.0f, 0.0f, 1 }, .color = {1,1,1,1} },
				{ .position = {-1.0f,  1.0f, 0.0f, 1 }, .color = {1,1,1,1} },
			};
			mVertics	= g3d::vertexbuffer(sizeof(vertexData));
			_queue.transfer()->update(mVertics, {vertexData, sizeof(vertexData)});
		}

		_queue.transfer()->update(mConstBuff, uniform_type
		{
			.camera	= _queue.projection,
			.world	= this->world_matrix(),
			.color	= this->get_color(),
		});


		auto		pcmd	= new g3d::cmdlist_pipeline(_queue.draw(), mPipeline);
		pcmd->bind(
			g3d::uniformset(mPipeline)
			.bind(0, mConstBuff)
		);
		pcmd->draw(mVertics, 0, 3);
		super_type::on_render(_queue);
	}

protected:
	g3d::material				mMaterial;
	g3d::constants				mConstBuff;
	g3d::vertexbuffer			mVertics;
	g3d::pipeline				mPipeline;
};



AMTRS_NAMESPACE_END
#endif
