/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__pipeline__hpp
#define	__libamtrs__graphics__g3d__pipeline__hpp
#include "renderstate.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


//! レンダリングパイプライン
class	pipeline
{
public:
	template<class Traits>
	using	realize_type	= typename Traits::pipeline_type;

	//! ドライバーが実体を保持するための構造体
	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		renderstate			rs;
		shaderset			mat;
	};


	pipeline() = default;
	pipeline(pipeline const&) = default;
	pipeline(pipeline&&) = default;
	pipeline& operator = (pipeline const&) = default;
	pipeline& operator = (pipeline&&) = default;

	//! _rs
	//!		empty() を渡した場合、直前に指定されたものを
	//!		再利用します。
	pipeline(shaderset _material, renderstate const& _rs);


	auto mat() const noexcept -> shaderset const& { return mInstance->mat; }
	auto layout() const noexcept -> shader_layout const& { return mat().layout(); }
	auto rstate() const noexcept -> renderstate const& { return mInstance->rs; }

	operator shaderset const& () const noexcept { return mat(); }
	operator shader_layout const& () const noexcept { return layout(); }


	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
