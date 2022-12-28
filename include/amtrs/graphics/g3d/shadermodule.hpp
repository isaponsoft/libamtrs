/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__shadermodule__hpp
#define	__libamtrs__graphics__g3d__shadermodule__hpp
#include "device.hpp"
#include "uniform_layout.hpp"
AMTRS_G3D_NAMESPACE_BEGIN
using namespace graph;


//! shader module
class	shadermodule
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>					realized;
		shader_stage					stage		= shader_stage::none_stage;
		shared_buffer<attribute>		attributes;
		uniform_layout					uniforms;
	};

	shadermodule() = default;
	shadermodule(shadermodule const&) = default;
	shadermodule(shadermodule&&) = default;
	shadermodule& operator = (shadermodule const&) = default;
	shadermodule& operator = (shadermodule&&) = default;


	/*!
	 * ステージとリソースを指定して初期化します。
	 * ----------------------------------------------------------------------
	 * シェーダーを作成します。ただし、シェーダーのコンパイルやロードは
	 * グラフィックスエンジン側の描画シーケンスの中で行われます。
	 * ----------------------------------------------------------------------
	 * _stage
	 *		シェーダーが対応するステージ。
	 * 		materialに設定時にこの値がデフォルト値として使用されます。
	 * _resource
	 *		グラフィックスエンジンがシェーダーをコンパイル／ロードするさいに
	 *		呼び出すコールバック。
	 *		void(void const* _data, size_t _dataSize) の関数オブジェクトが
	 *		引数に渡されます。
	 */
	shadermodule(shader_stage _stage);

	static shadermodule create_vertex(shared_buffer<attribute> _attributes, uniform_layout _uniforms = {});
	static shadermodule create_fragment(uniform_layout _uniforms = {});

	shader_stage stage() const noexcept
	{
		return	mInstance->stage;
	}

	auto attributes() const noexcept -> shared_buffer<attribute> const& { return mInstance->attributes; }
	auto uniforms() const noexcept -> uniform_layout const& { return mInstance->uniforms; }

	bool empty() const noexcept { return mInstance.empty(); }
private:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
