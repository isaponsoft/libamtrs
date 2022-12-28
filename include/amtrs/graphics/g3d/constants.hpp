/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__constants__hpp
#define	__libamtrs__graphics__g3d__constants__hpp
#include "shaderset.hpp"
AMTRS_G3D_NAMESPACE_BEGIN

// ****************************************************************************
//! 定数バッファ
// ----------------------------------------------------------------------------
//! uniformsetにbindして使用できます。
// ----------------------------------------------------------------------------
class	constants
{
public:
	struct	instance_type : ref_object
	{
		ref<ref_object>							realized;
		shader_layout							layout;
		uint32_t								index;
	};

	// ========================================================================
	//! レイアウトに対応する定数バッファを生成します。
	// ------------------------------------------------------------------------
	constants(shader_layout _layot, uint32_t _index);

	auto format() const noexcept -> shader_layout const& { return mInstance->layout; }

	bool empty() const noexcept { return mInstance.empty(); }

	constants() = default;
	constants(constants const&) = default;
	constants(constants&&) = default;
	constants& operator = (constants const&) = default;
	constants& operator = (constants&&) = default;
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
