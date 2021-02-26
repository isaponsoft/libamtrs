/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__sprite_node__hpp
#define	__libamtrs__node__sprite_node__hpp
#include "../g3d.hpp"
#include "../node.hpp"
AMTRS_NAMESPACE_BEGIN



template<class BaseT, class PixelT = rgba<uint8_t>>
class	sprite_node
		: public BaseT
{
public:
	using	super_type		= BaseT;
	using	pixel_type		= PixelT;
	using	image_type		= imagebuff<PixelT>;
	using	renderer_type	= typename super_type::renderer_type;
	using	shader_type		= g3d::basicshader::textureshader;
	using	uniform_type	= typename shader_type::uniform;
	using	vertex_type		= typename shader_type::vertex;
	using	super_type::super_type;

	sprite_node()
	{}

	template<class... Args>
	sprite_node(Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
	{}

	template<class... Args>
	sprite_node(image_type _image,  Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
	{
		set_image(std::move(_image));
	}

	void set_image(image_type _image, bool _update_size = true)
	{
		mImage		= std::move(_image);
		mTexture	= g3d::textureset();
		if (_update_size)
		{
			this->schedule_once([this]() { this->set_size(size3<float>(size2<float>(mImage.size()))); });
		}
	}


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
			mMaterial	= shader_type::create(_queue.transfer(), g3d::api_type::api_vulkan);
			mConstBuff	= g3d::constants(mMaterial, 0);
			mPipeline	= g3d::pipeline(mMaterial, {});
			mSampler	= g3d::imagesampler({
							.filter	= g3d::imagesampler::filter_liner,
							.access	= g3d::imagesampler::access_repeat,
						});
		}

		if (mTexture.empty())
		{
			mTexture	= g3d::textureset(size2<unsigned int>(mImage.size()));
			_queue.transfer()->update(mTexture, shared_buffer<void>(mImage.buffer()));
		}

		if (mVertics.empty())
		{
			auto				s			= this->size();
			const vertex_type	vertexData[] =
			{
				{ .position = {0, 0, 0, 1}, .color = {1,1,1,1}, .uv = {0,0} },
				{ .position = {s.width, 0, 0, 1}, .color = {1,1,1,1}, .uv = {1,0} },
				{ .position = {0, s.height, 0, 1}, .color = {1,1,1,1}, .uv = {0,1} },
				{ .position = {s.width, 0, 0.0f, 1 }, .color = {1,1,1,1}, .uv = {1,0} },
				{ .position = {s.width, s.height, 0.0f, 1 }, .color = {1,1,1,1}, .uv = {1,1} },
				{ .position = {0, s.height, 0.0f, 1 }, .color = {1,1,1,1}, .uv = {0,1} },
			};
			mVertics	= g3d::vertexbuffer(sizeof(vertexData));
			_queue.transfer()->update(mVertics, {std::begin(vertexData), std::end(vertexData)});
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
			.bind(1, mTexture, mSampler)
		);
		pcmd->draw(mVertics, 0, 6);

		super_type::on_render(_queue);
	}

protected:
	image_type				mImage;
	g3d::material			mMaterial;
	g3d::constants			mConstBuff;
	g3d::vertexbuffer		mVertics;
	g3d::pipeline			mPipeline;
	g3d::textureset			mTexture;
	g3d::imagesampler		mSampler;
};


AMTRS_NAMESPACE_END
#endif
