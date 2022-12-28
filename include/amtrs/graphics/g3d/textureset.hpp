/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__textureset__hpp
#define	__libamtrs__graphics__g3d__textureset__hpp
#include "type.hpp"
AMTRS_G3D_NAMESPACE_BEGIN


//! テクスチャを示します。
class	textureset
{
public:
	textureset() = default;
	textureset(textureset const&) = default;
	textureset(textureset&&) = default;
	textureset& operator = (textureset const&) = default;
	textureset& operator = (textureset&&) = default;

	enum class	format_types
	{
		rgba_u8		= 1,		//!< { u8, u8, u8, u8 } 
		rgba_f32	= 2,		//!< { float, float, float, float }
		f16			= 2,		//!< { float }
		f32			= 3,		//!< { float }
		s16			= 4,		//!< { s16 }
		u8			= 5,		//!< { u8 }
	};

	struct	instance_type : ref_object
	{
		ref<ref_object>		realized;
		size2<unsigned int>	size;
		format_types		format;		
		uint32_t			usage;
	};

	static constexpr uint32_t		usage_depth		= 1<<0;
	static constexpr uint32_t		usage_stencil	= 1<<1;

	static constexpr format_types	rgba_u8			= format_types::rgba_u8;
	static constexpr format_types	rgba_f32		= format_types::rgba_f32;
	static constexpr format_types	f16				= format_types::f16;
	static constexpr format_types	f32				= format_types::f32;
	static constexpr format_types	s16				= format_types::s16;
	static constexpr format_types	u8				= format_types::u8;

	// ========================================================================
	//! _size
	//!		usage に depth, stencil が指定されている場合、size に {0, 0}
	//!		を指定するとフレームバッファの大きさに合わせて自動的に調整されます。
	textureset(size2<unsigned int> _size, format_types _fmt = format_types::rgba_u8, uint32_t _usage = 0U);

	size2<unsigned int> const& size() const noexcept
	{
		return	mInstance->size;
	}

	format_types format() const noexcept
	{
		return	mInstance->format;
	}

	uint32_t usage() const noexcept
	{
		return	mInstance->usage;
	}

	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};

AMTRS_G3D_NAMESPACE_END
#endif
