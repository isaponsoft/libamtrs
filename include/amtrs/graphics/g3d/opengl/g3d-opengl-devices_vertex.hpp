/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__opengl__devices_vertex__hpp
#define	__libamtrs__graphics__opengl__devices_vertex__hpp
AMTRS_OPENGL_NAMESPACE_BEGIN

class	devices_vertex
		: public g3d::vertex_memory::memory_type
{
public:
	static constexpr GLuint		novalue	= 0;

	virtual ~devices_vertex()
	{
		release();
	}


	static bool realized(g3d::vertex_memory* _vtx) noexcept
	{
		return	_vtx->get_memory() != nullptr;
	}

	template<class Resources>
	static void realize(g3d::vertex_memory* _vtx, const vertex_layout& _layout, Resources* _resources)
	{
		ref<devices_vertex>	thiz	= new devices_vertex();
		_resources->set(thiz);
		thiz->mSize			= _vtx->size();
		thiz->mVertexLayout	= _layout;
		thiz->create();
		_vtx->set_memory(thiz);
	}


	static void active(g3d::vertex_memory* _vtx)
	{
		devices_vertex*	thiz	= static_cast<devices_vertex*>(_vtx->get_memory());
		glBindBuffer(GL_ARRAY_BUFFER, thiz->mVBO);
		AMTRS_G3D_OPENGL_GLGETERROR("glBindBuffer")

		auto	stride	= (GLsizei)thiz->mVertexLayout.length();
		for (const auto& attr : thiz->mVertexLayout)
		{
			glEnableVertexAttribArray(attr.location);
			AMTRS_G3D_OPENGL_GLGETERROR("glEnableVertexAttribArray")
			glVertexAttribPointer(attr.location, attr.count, attr.type, GL_FALSE, stride, (const void*)(std::uintptr_t)attr.offset);
			AMTRS_G3D_OPENGL_GLGETERROR("glVertexAttribPointer")
		}

		_vtx->transfer();
	}


	static void deactive(g3d::vertex_memory* _vtx)
	{
		devices_vertex*	thiz	= static_cast<devices_vertex*>(_vtx->get_memory());
		for (const auto& attr : thiz->mVertexLayout)
		{
			glDisableVertexAttribArray(attr.location);
			//AMTRS_G3D_OPENGL_GLGETERROR("glDisableVertexAttribArray")
		}
		glBindBuffer(GL_ARRAY_BUFFER, novalue);
		//AMTRS_G3D_OPENGL_GLGETERROR("glBindBuffer")
	}

	virtual void on_gain() override
	{
		create();
	}

	virtual void on_lost() override
	{
		release();
	}


	virtual void transfer(offset_type _destinate_offset, const void* _source, size_type _size) override
	{
		glBufferSubData(GL_ARRAY_BUFFER, _destinate_offset, _size, _source);
		AMTRS_G3D_OPENGL_GLGETERROR("glBufferSubData")
		std::memcpy(mRecoverBuffer.data() + _destinate_offset, reinterpret_cast<const char*>(_source), _size);
	}

private:
	void create()
	{
		mRecoverBuffer.resize(mSize);
		glGenBuffers(1, &mVBO);
		AMTRS_G3D_OPENGL_GLGETERROR("glGenBuffers");

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		AMTRS_G3D_OPENGL_GLGETERROR("glBindBuffer")

		glBufferData(GL_ARRAY_BUFFER, mRecoverBuffer.size(), mRecoverBuffer.data(), GL_STATIC_DRAW);
		AMTRS_G3D_OPENGL_GLGETERROR("glBufferData")

		glBindBuffer(GL_ARRAY_BUFFER, novalue);
		AMTRS_G3D_OPENGL_GLGETERROR("glBindBuffer")
	}

	void release()
	{
		if (mVBO != novalue)
		{
			glDeleteBuffers(1, &mVBO);
			//AMTRS_G3D_OPENGL_GLGETERROR("glDeleteBuffers")
			mVBO	= novalue;
		}
	}


	GLuint				mVBO				= novalue;
	std::vector<char>	mRecoverBuffer;
	vertex_layout		mVertexLayout;

};

AMTRS_OPENGL_NAMESPACE_END
#endif
