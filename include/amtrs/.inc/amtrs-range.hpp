/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__basic___range__hpp
#define	__libamtrs__basic___range__hpp
AMTRS_NAMESPACE_BEGIN

// ****************************************************************************
//! 最小値と最大値を持つデータ構造
// ----------------------------------------------------------------------------
template<class T>
struct	range
{
	using	value_type	= T;

	value_type	minimam;
	value_type	maximam;

	constexpr range() = default;
	constexpr range(const range&) = default;
	constexpr range(range&&) = default;
	constexpr range& operator = (const range&) = default;
	constexpr range& operator = (range&&) = default;

	constexpr range(value_type _min, value_type _max)
		: minimam(_min)
		, maximam(_max)
	{}

	template<class RT>
	explicit constexpr range(const range<RT>& _r)
		: minimam(static_cast<value_type>(_r.minimam))
		, maximam(static_cast<value_type>(_r.maximam))
	{}

	// ========================================================================
	//! 最小または最大値からはみ出している大きさを計算します。
	// ------------------------------------------------------------------------
	//! 範囲内に収まっている場合は 0 を返します。
	// ------------------------------------------------------------------------
	constexpr auto out_of(value_type _v) const noexcept -> value_type;

	bool operator == (const range& _r) const noexcept { return minimam == _r.minimam && maximam == _r.maximam; }
	bool operator != (const range& _r) const noexcept { return !(*this == _r); }
};


template<class T>
constexpr auto range<T>::out_of(value_type _v) const noexcept -> value_type
{
	if (_v < minimam)
	{
		return	_v - minimam;
	}
	if (_v > maximam)
	{
		return	_v - maximam;
	}
	return	0;
}

AMTRS_NAMESPACE_END
#endif
