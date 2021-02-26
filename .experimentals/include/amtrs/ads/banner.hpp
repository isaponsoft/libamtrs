/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__ads__banner__hpp
#define	__libamtrs__ads__banner__hpp
#include <string>
#include "def.hpp"
AMTRS_ADS_NAMESPACE_BEGIN

struct	providor
{
	enum	types
	{
		google_admob		= 1,
	};

	types	type;

	providor() = default;
	providor(providor const&) = default;
	providor(types _type)  : type(_type) {}
};

struct	google_admob_params : providor
{
	google_admob_params() : providor(google_admob) {}
	google_admob_params(std::string _unit_id) : providor(google_admob), unit_id(std::move(_unit_id)) {}

	std::string	unit_id;
};


class	banner
		: public ref_object
{
public:
	enum class	position_types
	{
		top		= 1,
		bottom	= 2,
	};
	static constexpr position_types	top		= position_types::top;
	static constexpr position_types	bottom	= position_types::bottom;

	static ref<banner> create(providor const* _prov, position_types _position = bottom);

	virtual void set_visibility(bool _visible) = 0;
};


AMTRS_ADS_NAMESPACE_END
#endif
