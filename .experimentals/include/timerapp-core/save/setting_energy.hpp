/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__setting_energy__hpp
#define	__libschedule__save__setting_energy__hpp
#include <amtrs/amtrs.hpp>
namespace save {

// ******************************************************************
//! エネルギーに関する設定
// ------------------------------------------------------------------
//!
// ==================================================================
struct	setting_energy
{
	static constexpr uint32_t	none_sleep_bits	= 1<<0;

	uint32_t	flags		= none_sleep_bits;


	bool none_sleep() const noexcept { return flags & none_sleep_bits; }
	void none_sleep(bool _on) noexcept { if (_on) { flags |= none_sleep_bits; } else { flags &= ~none_sleep_bits; } }
};

bool deserialize(setting_energy& _data, std::istream& _in);
bool serialize(std::ostream& _out, setting_energy const& _data);


}
#endif
