/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__g3d__imagesampler__hpp
#define	__libamtrs__graphics__g3d__imagesampler__hpp
#include "textureset.hpp"
AMTRS_G3D_NAMESPACE_BEGIN



//! テクスチャのサンプリング方法を指定します。
class	imagesampler
{
public:
	enum class	filter_modes
	{
		nearest	= 0,
		liner	= 1,
	};

	enum class	access_modes
	{
		repeat	= 0,
		mirror_repeat,
		clamp_edge,
		clamp_border,
	};

	static constexpr filter_modes	filter_nearest	= filter_modes::nearest;
	static constexpr filter_modes	filter_liner	= filter_modes::liner;
	static constexpr access_modes	access_repeat	= access_modes::repeat;
	static constexpr access_modes	access_mirror	= access_modes::mirror_repeat;
	static constexpr access_modes	access_edge		= access_modes::clamp_edge;
	static constexpr access_modes	access_border	= access_modes::clamp_border;

	struct	samplerdata
	{
		filter_modes	filter	= filter_nearest;
		access_modes	access	= access_repeat;
	};

	struct	instance_type : ref_object
	{
		ref<ref_object>	realized;
		samplerdata		data;
	};


	imagesampler() = default;
	imagesampler(imagesampler const&) = default;
	imagesampler(imagesampler&&) = default;
	imagesampler& operator = (imagesampler const&) = default;
	imagesampler& operator = (imagesampler&&) = default;

	imagesampler(samplerdata _data);

	bool empty() const noexcept { return mInstance.empty(); }
protected:
	ref<instance_type>	mInstance;

	template<class, class>
	friend	struct	realizer;
};


AMTRS_G3D_NAMESPACE_END
#endif
