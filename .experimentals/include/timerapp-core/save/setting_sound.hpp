/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__setting_sound__hpp
#define	__libschedule__save__setting_sound__hpp
#include <amtrs/amtrs.hpp>
namespace save {

// ******************************************************************
//! 音声に関する設定
// ------------------------------------------------------------------
//!
// ==================================================================
struct	setting_sound
{
	bool	systemSound		= true;			//!< システム音声の再生
	bool	homeTouchSound	= true;			//!< ホーム画面でのタッチ音声
};


bool deserialize(setting_sound& _data, std::istream& _in);
bool serialize(std::ostream& _out, setting_sound const& _data);

}
#endif
