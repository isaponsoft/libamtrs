/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libschedule__save__setting_event__hpp
#define	__libschedule__save__setting_event__hpp
#include <amtrs/amtrs.hpp>
namespace save {

// ******************************************************************
//! 
// ------------------------------------------------------------------
//!
// ==================================================================
struct	setting_event
{
	std::string	selectCalendar;
};


bool deserialize(setting_event& _data, std::istream& _in);
bool serialize(std::ostream& _out, setting_event const& _data);

}
#endif
