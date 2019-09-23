#ifndef	__libamtrs__graphics__opengl__texture_memory__hpp
#define	__libamtrs__graphics__opengl__texture_memory__hpp
AMTRS_OPENGL_NAMESPACE_BEGIN

class	texture_memory
		: public g3d::texture::memory_type
{
public:
	static constexpr GLuint	notex	= static_cast<GLuint>(0);

	virtual ~texture_memory() { release(); }


	void initialize(const g3d::device_capacity& _capacity, g3d::texture::format_type _format, size_type _size, unsigned int _miplevel)
	{
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
		mPixels			= bitmap<rgba<uint8_t>>(_size);
	}

	void release()
	{
		if (mTexture != notex)
		{
			glDeleteTextures(1, &mTexture);
			//AMTRS_G3D_OPENGL_GLGETERROR("glDeleteTextures")
			mTexture	= notex;
		}
	}

	virtual void on_gain() override
	{
		glActiveTexture(GL_TEXTURE0);
		AMTRS_G3D_OPENGL_GLGETERROR("glActiveTexture")
		create_texture();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mPixels.size().width, mPixels.size().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mPixels.pixels().data());
		AMTRS_G3D_OPENGL_GLGETERROR("glTexImage2D")
		mImaged	= true;
	}

	virtual void on_lost() override
	{
		release();
	}


	template<class Resources>
	static void active(GLuint _textureIndex, const g3d::device_capacity& _capacity, g3d::texture* _texture, Resources* _resources)
	{
		AMTRS_G3D_OPENGL_GLGETERROR("active")

		auto*	mapped		= _texture->mapped();
		auto*	memory		= mapped->get_memory() ? static_cast<texture_memory*>(mapped->get_memory()) : nullptr;

		glActiveTexture(_textureIndex);
		AMTRS_G3D_OPENGL_GLGETERROR("glActiveTexture")

		if (!memory)
		{
			ref<texture_memory>	retval	= new texture_memory();
			_resources->set(retval);
			retval->initialize(_capacity, mapped->format(), mapped->size(), mapped->miplevel());
			retval->create_texture();
			mapped->set_memory(retval);
			memory	= retval;
		}

		glBindTexture(GL_TEXTURE_2D, memory->mTexture);
		AMTRS_G3D_OPENGL_GLGETERROR("glBindTexture")

		mapped->transfer();
	}


	virtual void transfer(g3d::texture::offset_type _pos, const void* _texel_data, g3d::texture::size_type _size) override
	{
		AMTRS_G3D_OPENGL_GLGETERROR("transfer")

		if (_size.width == 0 || _size.height == 0 || _size.width > mSize.width || _size.height > mSize.height)
		{
			throw	std::runtime_error(amtrs::format<std::string>("glTexSubImage2D({%d,%d}, {%d,%d}) : invalid size", _pos.x, _pos.y, _size.width, _size.height));
		}

		// 復帰用のバッファへ転送
		auto					sr		= mPixels.subimg({_pos.x, _pos.y, _size.width, _size.height});
		const rgba<uint8_t>*	p		= (const rgba<uint8_t>*)_texel_data;
		for (auto& px : sr)
		{
			px = *p++;
		}

		if (!mImaged)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSize.width, mSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mPixels.pixels().data());
			AMTRS_G3D_OPENGL_GLGETERROR("glTexImage2D")
			mImaged	= true;
		}
		else
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _size.width, _size.height, GL_RGBA, GL_UNSIGNED_BYTE, _texel_data);
			AMTRS_G3D_OPENGL_GLGETERROR("glTexSubImage2D")
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
	g3d::texture::format_type	mFormat			= g3d::texture::rgba8;
	unsigned int				mMipmapLevel	= 1;
	bitmap<rgba<uint8_t>>		mPixels;
	bool						mImaged			= false;
};

AMTRS_OPENGL_NAMESPACE_END
#endif
