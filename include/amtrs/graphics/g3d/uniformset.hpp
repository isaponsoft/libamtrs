/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__uniformset__hpp
#define	__libamtrs__graphics__g3d__uniformset__hpp
#include "imagesampler.hpp"
#include "textureset.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


// ****************************************************************************
//! Uniformデータを保持します。
// ----------------------------------------------------------------------------
//! layout に従って定数データやテクスチャ（サンプラ）を保持します。
//! commandlistにこの値をbindすることでUniformを設定できます。
//! g3dエンジンはuniformオブジェクトを通すことでパイプラインの
//! 最適化を行うことができます。
// ----------------------------------------------------------------------------
class	uniformset
{
public:
	struct	instance_type
			: ref_object
	{
		struct	binddata : ref_object{};
		struct	constants_data : binddata
		{
			constants			val1;
		};
		struct	tetxure_data : binddata
		{
			textureset		val1;
			imagesampler	val2;
		};

		shader_layout					layout;
		bool							modified	= true;
		shared_buffer<ref<binddata>>	binds;
		ref<ref_object>					realized;
	};


	uniformset() = default;
	uniformset(uniformset const&) = default;
	uniformset(uniformset&&) = default;
	uniformset& operator = (uniformset const&) = default;
	uniformset& operator = (uniformset&&) = default;

	operator shader_layout const& () const noexcept { return layout(); }

	//! レイアウトを指定して作成します。
	uniformset(shader_layout _layout);

	//! 定数データをバインドします。
	uniformset& bind(uint32_t _index, constants _uniform);

	//! テクスチャとサンプラをバインドします。
	uniformset& bind(uint32_t _index, textureset _texture, imagesampler _sampler);

	auto layout() const noexcept -> shader_layout const&{ return mInstance->layout; }


	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
