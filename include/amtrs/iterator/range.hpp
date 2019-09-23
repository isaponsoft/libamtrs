/******************************************************************************
Copyright (c) 2019, isaponsoft (Isao Shibuya)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the libamtrs project.
******************************************************************************/
#ifndef	__libamtrs__amtrs_range__hpp
#define	__libamtrs__amtrs_range__hpp
#include "def.hpp"
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
