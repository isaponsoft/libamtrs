/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__renderstate__hpp
#define	__libamtrs__graphics__g3d__renderstate__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


// ****************************************************************************
//! レンダリングを行う時の描画方法を指定します。
// ----------------------------------------------------------------------------
//! 深度バッファの比較方法やカラーブレンドの方法などを指定します。
// ----------------------------------------------------------------------------
class	renderstate
{
public:
	//! カリング
	enum class	culling_modes
	{
		none	= 0,
		front	= 1,
		back	= 2,
	};
	static constexpr culling_modes	cull_mode_none	= culling_modes::none;
	static constexpr culling_modes	cull_mode_front	= culling_modes::front;
	static constexpr culling_modes	cull_mode_back	= culling_modes::back;


	//! 比較方法
	enum class	compare_modes
	{
		never	= 0,			//!< 常に不合格
		always,					//!< 常に合格
		equal,
		not_equal,
		less,
		less_or_equal,
		grater,
		grater_or_equal,
	};
	static constexpr compare_modes	compare_never			= compare_modes::never;
	static constexpr compare_modes	compare_always			= compare_modes::always;
	static constexpr compare_modes	compare_equal			= compare_modes::equal;
	static constexpr compare_modes	compare_not_equal		= compare_modes::not_equal;
	static constexpr compare_modes	compare_less			= compare_modes::less;
	static constexpr compare_modes	compare_less_or_equal	= compare_modes::less_or_equal;
	static constexpr compare_modes	compare_grater			= compare_modes::grater;
	static constexpr compare_modes	compare_grater_or_equal	= compare_modes::grater_or_equal;

	//! ステンシルOp
	enum class	stensil_operators
	{
		keep		= 0,
		zero,
		replace,
		invert,
		inc_and_clamp,
		inc_and_wrap,
		dec_and_clamp,
		dec_and_wrap,
	};
	static constexpr stensil_operators	stencil_op_keep				= stensil_operators::keep;
	static constexpr stensil_operators	stencil_op_zero				= stensil_operators::zero;
	static constexpr stensil_operators	stencil_op_replace			= stensil_operators::replace;
	static constexpr stensil_operators	stencil_op_invert			= stensil_operators::invert;
	static constexpr stensil_operators	stencil_op_inc_and_clamp	= stensil_operators::inc_and_clamp;
	static constexpr stensil_operators	stencil_op_inc_and_wrap		= stensil_operators::inc_and_wrap;
	static constexpr stensil_operators	stencil_op_dec_and_clamp	= stensil_operators::dec_and_clamp;
	static constexpr stensil_operators	stencil_op_dec_and_wrap		= stensil_operators::dec_and_wrap;


	//! depth test
	struct	depth_test
	{
		bool			enable	= true;
		bool			write	= true;
		compare_modes	compare	= compare_modes::less;
	};


	//! stencil test
	struct	stencil_test
	{
		bool				enable		= false;
		compare_modes		compare		= compare_modes::always;
		stensil_operators	stencil_fail= stensil_operators::keep;
		stensil_operators	depth_fail	= stensil_operators::keep;
		stensil_operators	depth_pass	= stensil_operators::keep;
		uint32_t			compareMask	= 0xff;
		uint32_t			writeMask	= 0xff;
		uint32_t			reference	= 1;
	};



	struct	status
	{
		culling_modes	culling	= cull_mode_none;
		depth_test		depth;
		stencil_test	stencil;
	};

	//! ドライバーが実体を保持するための構造体
	struct	instance_type : ref_object, status
	{
		instance_type(status&& _s)
			: status(std::move(_s))
		{}

		ref<ref_object>		realized;
	};


	renderstate() = default;
	renderstate(renderstate const&) = default;
	renderstate(renderstate&&) = default;
	renderstate& operator = (renderstate const&) = default;
	renderstate& operator = (renderstate&&) = default;

	//! _depth
	//!		深度テストの方法を指定します。
	//!		nullptr を指定した場合、 enable = false を指定したのと
	//!		同じ扱いになります。
	renderstate(status _status)
	{
		mInstance	= new instance_type(std::move(_status));
	}

	renderstate& operator = (status _s)
	{
		mInstance	= new instance_type(std::move(_s));
		return	*this;
	}


	bool empty() const noexcept { return mInstance.empty(); }

protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};

AMTRS_G3D_NAMESPACE_END
#endif
