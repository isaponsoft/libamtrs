/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__framebuffer__hpp
#define	__libamtrs__graphics__g3d__framebuffer__hpp
#include "textureset.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


//! 書き込み先のVRAMを指定します。
//! カラーバッファ、深度バッファ、Zバッファの組み合わせです。
class	framebuffer
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>	realized;
		textureset		color;
		textureset		depth;
		textureset		stencil;
	};

	//! _color に nullptr を指定した場合、描画先としてスワップチェーンに関連付けされている
	//! カラーバッファ（≒ウィンドウ）を対象とするようにドライバに伝えます。
	//! 1枚のテクスチャでdepth と stencil を補えるデバイスがありますがその場合は
	//! depthとstencilに同じテクスチャを指定してください。
	framebuffer(textureset _color, textureset _depth, textureset _stencil);

	framebuffer() = default;
	framebuffer(framebuffer const&) = default;
	framebuffer(framebuffer&&) = default;
	framebuffer& operator = (framebuffer const&) = default;
	framebuffer& operator = (framebuffer&&) = default;

	textureset const& color() const noexcept { return mInstance->color; }
	textureset const& depth() const noexcept { return mInstance->depth; }
	textureset const& stencil() const noexcept { return mInstance->stencil; }

	bool empty() const noexcept { return mInstance.empty(); }
	void reset();
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
