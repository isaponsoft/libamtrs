/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__device__opengl__hpp
#define	__libamtrs__g3d__device__opengl__hpp
#include <vector>
#include <list>
#include "../../application.hpp"
#include "opengl/attribute.hpp"
#include "opengl/def.hpp"
#include "opengl/gl_error.hpp"
#include "opengl/gl_types.hpp"
#include "opengl/notify.hpp"
#include "opengl/shader.hpp"
#include "opengl/texture_memory.hpp"
#include "opengl/vertex_memory.hpp"
AMTRS_OPENGL_NAMESPACE_BEGIN 




// ============================================================================
//! OpenGL のコンテキストを抽象化したインターフェースです。
// ----------------------------------------------------------------------------
//! コンテキストの作成方法はＯＳによって異なります。
//! Win32なら amtrs::os::win32::opengl、Android なら amtrs::os::android::opengl
//! などにあるコンテキストヘルパで生成できます。
// ----------------------------------------------------------------------------
class	device
		: public g3d::device
{
public:
	using	device_lost_func	= void(bool);

	device()
		: g3d::device(opengl)
	{}

	~device()
	{
		on_lost();
	}


	//! コンテキストが有効か調べます。
	virtual bool is_context_enable() const override
	{
		return	mOnActive;
	}

	virtual void present() override
	{
		mResources.cleanup();
	}

	// ============================================================================
	//! パイプラインを描画します。
	// ----------------------------------------------------------------------------
	virtual void draw(g3d::draw_command* _command) override
	{
		auto&	cmd	= *_command;
		if (cmd.shader.empty())
		{
			return;
		}
		mUsingShader	= static_cast<shader*>(cmd.shader.get());
		mUsingShader->load_resource(*capacity(), cmd.get_uniform(), &mResources);
		mUsingShader->use_program();
		mUsingShader->transfer_uniform(*capacity(), cmd.get_uniform());


		// 頂点メモリをアクティブ化
		if (!vertex_memory::realized(cmd.vertex))
		{
			vertex_memory::realize(cmd.vertex, mUsingShader->get_vertex_layout(), &mResources);
		}
		vertex_memory::active(cmd.vertex);


		// 各種設定を変更
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// アルファブレンド
		if (cmd.status.alpha_blend)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		glBlendFunc(get_blend_mode(cmd.status.src_blend), get_blend_mode(cmd.status.dst_blend));

		// テクスチャフィルタ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_texture2d_flter(cmd.status.texture2d_min_filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_texture2d_flter(cmd.status.texture2d_mag_filter));


		// 頂点を描画
		GLuint	prim	= cmd.primitive == triangles      ? GL_TRIANGLES
						: cmd.primitive == triangle_strip ? GL_TRIANGLE_STRIP
						: cmd.primitive == triangle_fan   ? GL_TRIANGLE_FAN
						: GL_POINTS;
		glDrawArrays(prim, (GLsizei)cmd.first, (GLsizei)cmd.count);

		// 頂点の後片付け
		vertex_memory::deactive(cmd.vertex);

		mUsingShader->deactive();
		mUsingShader	= nullptr;
	}


	virtual std::string name() const override
	{
		std::string	retval;
		auto	render	= reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		retval	+= (render ? render : "unkown render");
		return	retval;
	}


	virtual ref<g3d::shader> create_shader(const attribute* _attr, std::size_t _attr_size, const attribute* _uniform, std::size_t _uniform_size, std::string_view _vertex, std::string_view _fragment) override
	{
		ref<shader>	retval	= new shader();
		mResources.set(retval);
		retval->initialize(_vertex, _fragment);
		retval->initialize_attribute_info(_attr, _attr_size);
		retval->initialize_uniform_info(_uniform, _uniform_size);
		return	retval;
	}


protected:
	//! デバイスを入手可能な状態になった時に派生先から呼び出します。
	virtual void on_gain()
	{
		// デバイスのキャパシティを取得／保存
		auto&	cap	= *capacity();
		GLint	tmp1;

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tmp1);
		cap.maxTextureSize = tmp1;
		//AMTRS_DEBUG_LOG("max texture size = %d", mMaxTextureSize);

		const char*	extensions	= (char*)glGetString(GL_EXTENSIONS); 
		if (strstr(extensions, "non_power_of_two"))
		{
			cap.textureNonPowerOfTwo	= true;		AMTRS_DEBUG_LOG("Support : non 2^x texture");
		}

		// リソース各種に状態変更を通知
		for (auto r : mResources) { r->on_gain(); }
		mOnActive		= true;
	}

	//! デバイスを失った状態になった時に派生先から呼び出します。
	virtual void on_lost()
	{
		for (auto r : mResources)
		{
			r->on_lost();
		}
		mUsingShader	= nullptr;
		mOnActive		= false;
	}


private:
	ref<shader>						mUsingShader;
	g3d::draw_status				mLastDrawStatus;
	resorces						mResources;
	bool							mOnActive	= false;
};




AMTRS_OPENGL_NAMESPACE_END
#endif
