/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__vertexbuffer__hpp
#define	__libamtrs__graphics__g3d__vertexbuffer__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

// ****************************************************************************
//! 頂点バッファ
// ----------------------------------------------------------------------------
class	vertexbuffer
{
public:
	//! ドライバーが実体を保持するための構造体
	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		size_t				size;
	};

	vertexbuffer() = default;
	vertexbuffer(vertexbuffer const&) = default;
	vertexbuffer(vertexbuffer&&) = default;
	vertexbuffer& operator = (vertexbuffer const&) = default;
	vertexbuffer& operator = (vertexbuffer&&) = default;


	vertexbuffer(size_t _size);

	//! バッファのソースデータのサイズ取得します。
	auto size() const noexcept { return mInstance->size; }

	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};

AMTRS_G3D_NAMESPACE_END
#endif
