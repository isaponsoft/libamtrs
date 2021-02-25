/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__platform__opengl__g3d_device__hpp
#define	__libamtrs__platform__opengl__g3d_device__hpp
#include <vector>
#include <list>
#include "../application.hpp"
#include "g3d-opengl-attribute.hpp"
#include "g3d-opengl-def.hpp"
#include "g3d-opengl-gl_error.hpp"
#include "g3d-opengl-gl_types.hpp"
#include "g3d-opengl-devices_shader.hpp"
#include "g3d-opengl-devices_texture.hpp"
#include "g3d-opengl-devices_vertex.hpp"
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

		// シェーダー本体を取得/コンパイル
		auto	shader	= devices_shader::get_shader(cmd.shader, &mResources);

		// テクスチャをロードする
		shader->enumrate_textures(cmd.get_uniform(), [&](GLuint index, g3d::texture* tex)
		{
			devices_texture::active(GL_TEXTURE0 + index, *capacity(), tex, &mResources);
		});

		// 頂点データをロードする
		if (!devices_vertex::realized(cmd.vertex))
		{
			devices_vertex::realize(cmd.vertex, shader->vtxlayout(), &mResources);
		}


		// シェーダーをアクティブ化
		shader->activate(*capacity(), cmd.get_uniform());

		// 頂点をアクティブ化
		devices_vertex::active(cmd.vertex);



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
		devices_vertex::deactive(cmd.vertex);

		shader->deactivate();
	}


	virtual std::string name() const override
	{
		std::string	retval;
		auto	render	= reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		retval	+= (render ? render : "unkown render");
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
		mOnActive		= false;
	}


private:
	g3d::draw_status				mLastDrawStatus;
	resorces						mResources;
	bool							mOnActive	= false;
};




AMTRS_OPENGL_NAMESPACE_END
#endif
