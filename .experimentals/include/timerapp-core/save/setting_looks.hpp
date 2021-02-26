/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__setting_looks__hpp
#define	__libschedule__save__setting_looks__hpp
#include <string>
#include <amtrs/amtrs.hpp>
#include <amtrs/graphics/color.hpp>
namespace save {

// ******************************************************************
//! 見た目に関する設定
// ------------------------------------------------------------------
//!
// ==================================================================
struct	setting_looks
{
	enum	bgmodes : int8_t
	{
		bgmode_none		= 0,
		bgmode_image	= 1,
		bgmode_color	= 2,
	};

	static constexpr uint32_t	schedule_show_seconds_bits	= 1<<0;


	int8_t			mode			= bgmodes::bgmode_none;
	std::string		bgimage;							//!< 背景画像の名称
	rgb<uint8_t>	color			= {0,0,0};			//!< 背景の色

	int16_t			transpalency	= 100;				//!< 不透明度(パーセンテージ)


	uint32_t		flags			= schedule_show_seconds_bits
									;

	bool schedule_show_seconds() const noexcept { return flags & schedule_show_seconds_bits; }
	void schedule_show_seconds(bool _on) noexcept { if (_on) { flags |= schedule_show_seconds_bits; } else { flags &= ~schedule_show_seconds_bits; } }
};

bool deserialize(setting_looks& _data, std::istream& _in);
bool serialize(std::ostream& _out, setting_looks const& _data);


}
#endif
