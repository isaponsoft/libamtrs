/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__shaderset__hpp
#define	__libamtrs__graphics__g3d__shaderset__hpp
#include "shader_layout.hpp"
AMTRS_G3D_NAMESPACE_BEGIN



// ****************************************************************************
//! マテリアル
// ----------------------------------------------------------------------------
//! シェーダーと入力データのレイアウトを表します。
//! マテリアルの内容が変更されると描画パイプラリンが再構築されるためロスタイムが
//! 発生します。
// ----------------------------------------------------------------------------
class	shaderset
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>	realized;
		shader_layout	layout;
		shadermodule	modules[(size_t)shader_stage::_max];
	};

	shaderset(std::initializer_list<shadermodule> _shaders);
	shaderset(shader_layout _format, std::initializer_list<shadermodule> _shaders);

	auto shader(shader_stage _stage) const noexcept -> shadermodule const& { return mInstance->modules[(int)_stage]; }
	auto layout() const noexcept -> shader_layout const& { return mInstance->layout; }
	bool empty() const noexcept { return mInstance.empty(); }

	shaderset() = default;
	shaderset(shaderset const&) = default;
	shaderset(shaderset&&) = default;
	shaderset& operator = (shaderset const&) = default;
	shaderset& operator = (shaderset&&) = default;
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
