/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__node__camera_node__hpp
#define	__libamtrs__node__camera_node__hpp
#include "../media/camera.hpp"
#include "anchor_node.hpp"
#include "sprite_node_yuv420.hpp"
AMTRS_NAMESPACE_BEGIN


template<class SuperT>
class	camera_node
		: public SuperT
{
public:
	using	image_type	= media::camera::image_type;
	using	super_type	= SuperT;


	template<class... Args>
	camera_node(Args&&... _args)
		: super_type(std::forward<Args>(_args)...)
	{
		auto	s	= new anchor_node<sprite_node_yuv420<node>>();
		s->set_anchor({0.5f, 0.5f, 0});
		mSprite	= this->add_child(s);
		mSprite->set_pivot({0.5f, 0.5f, 0});
		mSprite->set_rotate(quaternion<float>::rotate_z(degrees_to_radians(-90)));
	}

	~camera_node()
	{
		stop();
	}

	void start(ref<media::camera> _camera, media::camera::format _format)
	{
		mCamera	= std::move(_camera);
		mCapturing	= true;
		mCamera->start(_format, [this](auto planes, auto size)
		{
			if (!this->mCapturing) { return; }
			auto	y	= create_imagebuff(imageview<void const>(planes[0]), color_format::gray8);
			auto	u	= create_imagebuff(imageview<void const>(planes[1]), color_format::gray8);
			auto	v	= create_imagebuff(imageview<void const>(planes[2]), color_format::gray8);
			if (!this->mCapturing) { return; }
			this->schedule_once([this, _y = std::move(y), _u = std::move(u), _v = std::move(v)]()
			{
				if (!this->mCapturing) { return; }
				this->on_capture(_y, _u, _v);
			});
		});
	}

	void pause()
	{
		if (mCamera && mCapturing)
		{
			mCapturing	= false;
			mCamera->pause();
		}
	}

	void restart()
	{
		if (mCamera && !mCapturing)
		{
			mCapturing	= true;
			mCamera->restart();
		}
	}

	void stop()
	{
		if (mCamera)
		{
			mCapturing	= false;
			mCamera->stop();
			mCamera.clear();
		}
	}

	bool capturing() const noexcept { return mCapturing; }

protected:
	virtual void on_exit() override
	{
		mCapturing	= false;
		super_type::on_exit();
		stop();
	}

	virtual void on_size_change(typename super_type::size_type& _size) override
	{
		super_type::on_size_change(_size);
		update_previewsize();
	}

	virtual void on_capture(imagebuff<uint8_t> const& Y, imagebuff<uint8_t> const& U, imagebuff<uint8_t> const& V)
	{
		auto	texY	= g3d::texture::create(Y.share(), g3d::texture::norestore);
		auto	texU	= g3d::texture::create(U.share(), g3d::texture::norestore);
		auto	texV	= g3d::texture::create(V.share(), g3d::texture::norestore);
		mSprite->set_texture(texY, texU, texV);
		update_previewsize();
	}

	void update_previewsize()
	{
		if (!mUpdateDirty)
		{
			mUpdateDirty	= true;
			this->schedule_once([this]()
			{
				if (auto parent = this->parent(); parent)
				{
					auto	ss		= mSprite->size();
					auto	ps		= parent->size();
					float	scale	= std::max<float>(ps.height / ss.width, ps.width / ss.height);
					mSprite->set_scale({scale, scale, 0});
				}
				mUpdateDirty	= false;
			});
		}
	}

private:
	ref<media::camera>				mCamera;
	sprite_node_yuv420<node>*		mSprite;
	volatile bool					mCapturing	= false;
	bool							mUpdateDirty	= false;
};

AMTRS_NAMESPACE_END
#endif
