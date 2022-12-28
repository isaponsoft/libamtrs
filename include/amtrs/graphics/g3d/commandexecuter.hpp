/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__commandexecuter__hpp
#define	__libamtrs__graphics__g3d__commandexecuter__hpp
#include "constants.hpp"
#include "framebuffer.hpp"
#include "pipeline.hpp"
#include "shadermodule.hpp"
#include "scissor.hpp"
#include "uniformset.hpp"
#include "textureset.hpp"
#include "vertexbuffer.hpp"
#include "viewport.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


class	commandexecuter
{
public:
	//! Uniform データの更新を行います。
	struct	constants_update
	{
		constants			target;
		shared_buffer<void>	source;
	};

	//! Uniform データの更新を行います。
	struct	vertexbuffer_update
	{
		vertexbuffer		target;
		shared_buffer<void>	source;
	};

	//! textureset データの更新を行います。
	struct	textureset_update
	{
		textureset			target;
		shared_buffer<void>	source;
	};

	//! shadermodule の更新を行います。
	struct	shadermodule_update
	{
		shadermodule		target;
		shared_buffer<void>	source;
	};

	//! デフォルトのレンダリングステータスをPushします。
	struct	renderstatus_push
	{
		renderstate			state;		// empty() の場合は pop します。
	};

	//! レンダリングパスを開始します。
	struct	renderpass_begin
	{
		framebuffer			target;
		rgba<float>			color;
		float				depth;
		float				stencil;
	};

	//! レンダリングパスを終了します。
	struct	renderpass_end
	{
	};

	//! パイプラインを変更します。
	struct	pipeline_change
	{
		pipeline			pipe;
	};

	//! テクスチャをカレントパイプラインに設定します。
	struct	uniform_bind
	{
		uniformset			u;
	};

	//! 頂点データを描画します。
	//! インデックス情報を使用する場合は draw_inddicis を使用してください。
	struct	draw_vertex
	{
		vertexbuffer		target;
		uint32_t			offset;
		uint32_t			count;
	};

	struct	viewport_scissor
	{
		uint32_t			flags;		// 0x01: viewport, 0x02 : scissor
		struct viewport		viewport;
		struct scissor		scissor;
	};

	struct	begin
	{
	};


	struct	end
	{
	};


	//! 画面に対してレンダリング結果を反映させます。
	struct	present
	{
	};



	virtual void exec(constants_update& _data) = 0;
	virtual void exec(vertexbuffer_update& _data) = 0;
	virtual void exec(textureset_update& _data) = 0;
	virtual void exec(shadermodule_update& _data) = 0;
	virtual void exec(renderstatus_push& _data) = 0;
	virtual void exec(renderpass_begin& _data) = 0;
	virtual void exec(renderpass_end& _data) = 0;
	virtual void exec(pipeline_change& _data) = 0;
	virtual void exec(viewport_scissor& _data) = 0;
	virtual void exec(uniform_bind& _data) = 0;
	virtual void exec(draw_vertex& _data) = 0;
	virtual void exec(begin& _data) = 0;
	virtual void exec(end& _data) = 0;
	virtual void exec(present& _data) = 0;

};

AMTRS_G3D_NAMESPACE_END
#endif
