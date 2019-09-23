/*-
 * Copyright (c) 2017 Isao Shibuya
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php 
 */
#ifndef	__libamtrs__g3d__draw_status__hpp
#define	__libamtrs__g3d__draw_status__hpp
#include "def.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


// ============================================================================
//! 描画する際の設定
// ----------------------------------------------------------------------------
struct	draw_status
{
	static constexpr bool	enable	= true;
	static constexpr bool	disable	= false;


	// ========================================================================
	// アルファブレント
	// ------------------------------------------------------------------------


	// 名称は Direct3D に合わせています。
	// 	https://msdn.microsoft.com/ja-jp/library/cc324307.aspx
	enum class	blend_type
	{
		zero				= 1,			//!< (0, 0, 0, 0)
		one					= 2,			//!< (1, 1, 1, 1)
		src_color			= 3,			//!< (Rs, Gs, Bs, As)
		invrs_src_color		= 4,			//!< (1 - Rs, 1 - Gs, 1 - Bs, 1 - As)
		src_alpha			= 5,			//!< (As, As, As, As)
		invrs_src_alpha		= 6,			//!< (1 - As, 1 - As, 1 - As, 1 - As)
		dest_color			= 7,			//!< (Rd, Gd, Bd, Ad)
		invrs_dest_color	= 8,			//!< (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)
		dest_alpha			= 9,			//!< (Ad, Ad, Ad, Ad)
		invrs_dest_alpha	= 10,			//!< (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad)
	};

	bool				alpha_blend	= enable;
	blend_type			src_blend	= blend_type::src_alpha;
	blend_type			dst_blend	= blend_type::invrs_src_alpha;

	constexpr void set_alpha_blend(blend_type _src, blend_type _dst) noexcept
	{
		alpha_blend	= enable;
		src_blend	= _src;
		dst_blend	= _dst;
	}



	// ========================================================================
	// テクスチャサンプラー
	// ------------------------------------------------------------------------

	// 名称は Direct3D に合わせています。
	// 	https://msdn.microsoft.com/en-us/library/windows/desktop/dn770367(v=vs.85).aspx
	enum class	texture2d_filter_type
	{
		point				= 0,			//!< OpenGL = NEAR
		liner				= 1,			//!< OpenGL = Liner
		anisotropic			= 2,			//!< 異方性フィルタリング。 OpenGL の場合は拡張モードが有効な場合に有効になる。
	};

	texture2d_filter_type	texture2d_min_filter	= texture2d_filter_type::liner;
	texture2d_filter_type	texture2d_mag_filter	= texture2d_filter_type::liner;

	constexpr void set_texture2d_filter(texture2d_filter_type _min, texture2d_filter_type _mag) noexcept
	{
		texture2d_min_filter	= _min;
		texture2d_mag_filter	= _mag;
	}
};


//! draw_status をドット絵の表示向けの設定にする
inline draw_status& set_pixelart_settings(draw_status& _ds)
{
	_ds.alpha_blend	= false;
	_ds.set_texture2d_filter
	(
		draw_status::texture2d_filter_type::point,
		draw_status::texture2d_filter_type::point
	);
	return	_ds;
}



AMTRS_G3D_NAMESPACE_END
#endif
