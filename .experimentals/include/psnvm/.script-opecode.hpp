/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__psnvm__script_opecode__hpp
#define	__psnvm__script_opecode__hpp
#include "def.hpp"
PASSION_NAMESPACE_BEGIN

// ****************************************************************************
//! 演算子の優先順位
// ----------------------------------------------------------------------------
enum class opecode_priority
{
	none,						//!< 0: 値(優先順位には影響しない)
	assign,						//!< 1: = += -= *= /= %= &= |= ^= <<= >>=
	conditions = assign,		//!< 1: ?:
	logic_and,					//!< 2: && ||
	compare,					//!< 3: == != <= >= < >
	bitwise,					//!< 4: & | ^
	shift,						//!< 5: << >>
	add_sub,					//!< 6: + -
	mul_div,					//!< 7: * / %
	single_operation,			//!< 8: ! ~ - *pointer &pointer
	primary,					//!< 9: () や [] など最優先のもの
};



PASSION_NAMESPACE_END
#endif
