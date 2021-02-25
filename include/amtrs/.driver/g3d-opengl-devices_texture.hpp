/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__opengl__devices_texture__hpp
#define	__libamtrs__graphics__opengl__devices_texture__hpp
AMTRS_OPENGL_NAMESPACE_BEGIN

class	devices_texture
		: public g3d::texture::memory_type
{
public:
	static constexpr GLuint	notex	= static_cast<GLuint>(0);

	virtual ~devices_texture() { release(); }


	void initialize(const g3d::device_capacity& _capacity, color_format _format, size_type _size, unsigned int _miplevel, unsigned int _flags)
	{
		this->mFlags	= _flags;
		this->mSize		= _size;
		if (!_capacity.textureNonPowerOfTwo)
		{
			// ２の乗数にする
			mSize		= {2, 2};
			while (mSize.width  < _size.width ) { mSize.width  <<= 1; }
			while (mSize.height < _size.height) { mSize.height <<= 1; }
		}
		mFormat			= _format;
		mMipmapLevel	= _miplevel;

		size_t	stridex	= pixel_size(mFormat);
		if (!(_flags & g3d::texture::norestore))
		{
			mRestoreData	= shared_buffer<uint8_t>(stridex * _size.width * _size.height);
		}
		mRestoreView.size(_size);
		mRestoreView.stride({(int)stridex, (int)(stridex * _size.width)});
		mRestoreView.data(mRestoreData.data());
	}

	void release()
	{
		if (mTexture != notex)
		{
			glDeleteTextures(1, &mTexture);
			//AMTRS_G3D_OPENGL_GLGETERROR("glDeleteTextures")
			mTexture	= notex;
			mImaged		= false;
		}
	}

	virtual void on_gain() override
	{
	}

	virtual void on_lost() override
	{
		release();
	}


	template<class Resources>
	static bool active(GLuint _textureIndex, const g3d::device_capacity& _capacity, g3d::texture* _texture, Resources* _resources)
	{
		if (!_texture)
		{
			AMTRS_DEBUG_LOG("Texture is null");
			return	false;
		}

		AMTRS_G3D_OPENGL_GLGETERROR("active")

		auto*	mapped		= _texture->mapped();
		if (!mapped)
		{
			AMTRS_DEBUG_LOG("Texture mapped error")
			return	false;
		}

		ref<devices_texture>	thiz		= mapped->get_memory() ? static_cast<devices_texture*>(mapped->get_memory()) : nullptr;

		glActiveTexture(_textureIndex);
		AMTRS_G3D_OPENGL_GLGETERROR("glActiveTexture")
		if (!thiz)
		{
			thiz	= new devices_texture();
			_resources->set(thiz);
			auto				fmt		= mapped->format();
			auto				siz		= mapped->size();
			auto				mip		= mapped->miplevel();
			thiz->initialize(_capacity, fmt, siz, mip, mapped->flags());
			mapped->set_memory(thiz);
		}
		if (thiz->mTexture == notex)
		{
			thiz->create_texture();
			if (!thiz->mRestoreData.empty())
			{
				GLuint	fmt	= thiz->mFormat == color_format::rgba8 ? GL_RGBA
							: thiz->mFormat == color_format::rgbx8 ? GL_RGBA
							: thiz->mFormat == color_format::rgb8  ? GL_RGB
							: thiz->mFormat == color_format::gray8 ? GL_LUMINANCE
							:                            	         GL_LUMINANCE;
				glTexImage2D(GL_TEXTURE_2D, 0, fmt, thiz->mRestoreView.size().width, thiz->mRestoreView.size().height, 0, fmt, GL_UNSIGNED_BYTE, thiz->mRestoreData.data());
				thiz->mImaged	= true;
			}
		}
		glBindTexture(GL_TEXTURE_2D, thiz->mTexture);
		AMTRS_G3D_OPENGL_GLGETERROR("glBindTexture")

		mapped->transfer();
		return	true;
	}


	virtual void transfer(g3d::texture::offset_type _pos, const void* _texel_data, g3d::texture::size_type _size) override
	{
		AMTRS_G3D_OPENGL_GLGETERROR("transfer")

		if (_size.width == 0 || _size.height == 0 || _size.width > mSize.width || _size.height > mSize.height)
		{
			throw	std::runtime_error(amtrs::format<std::string>("glTexSubImage2D({%d,%d}, {%d,%d}) : invalid size", _pos.x, _pos.y, _size.width, _size.height));
		}

		// 復帰用のバッファへ転送

		GLuint		fmt	= mFormat == color_format::rgba8 ? GL_RGBA
						: mFormat == color_format::rgbx8 ? GL_RGBA
						: mFormat == color_format::rgb8  ? GL_RGB
						: mFormat == color_format::gray8 ? GL_LUMINANCE
						:                                  GL_LUMINANCE;
		if (!(mFlags & g3d::texture::norestore))
		{
			size_t			ss	= mRestoreView.stride().x * _size.width;
			size_t			di	= mRestoreView.stride().y * _pos.y + mRestoreView.stride().x * _pos.x;
			uint8_t const*	s	= reinterpret_cast<uint8_t const*>(_texel_data);
			uint8_t*		d	= mRestoreData.data() + di;
			for (int y = 0; y < (int)_size.height; ++y)
			{
				std::memcpy(d + di, s, ss);
				s	+=  ss;
				di	+= mRestoreView.stride().y;
			}
			if (!mImaged)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, fmt, mSize.width, mSize.height, 0, fmt, GL_UNSIGNED_BYTE, mRestoreData.data());
				AMTRS_G3D_OPENGL_GLGETERROR("glTexImage2D")
				mImaged	= true;
			}
			else
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _size.width, _size.height, fmt, GL_UNSIGNED_BYTE, _texel_data);
				AMTRS_G3D_OPENGL_GLGETERROR("glTexSubImage2D")
			}
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, fmt, _size.width, _size.height, 0, fmt, GL_UNSIGNED_BYTE, _texel_data);
			AMTRS_G3D_OPENGL_GLGETERROR("glTexImage2D")
			mImaged	= true;
		}
	}


private:
	void create_texture()
	{
		glGetError();
		glGenTextures(1, &mTexture);
		AMTRS_G3D_OPENGL_GLGETERROR("glGenTextures")

		glBindTexture(GL_TEXTURE_2D, mTexture);
		AMTRS_G3D_OPENGL_GLGETERROR("glBindTexture")

#if 1
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	GLuint						mTexture		= notex;
	color_format				mFormat			= color_format::rgba8;
	unsigned int				mMipmapLevel	= 1;
	unsigned int				mFlags			= 0;

	imageview<uint8_t>			mRestoreView;
	shared_buffer<uint8_t>		mRestoreData;
	bool						mImaged			= false;
};

AMTRS_OPENGL_NAMESPACE_END
#endif
