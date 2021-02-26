/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__variant__h
#define	__passion__variant__h
#include "integer.h"

//! 
typedef	struct	_passion_variable_size
		{
			uint16_t		static_size;		//!< 静的に確定したサイズ
			uint16_t		int_count;			//!< INT型の数
		} passion_variable_size;


union	passion_variant
{
	struct passion_object*		o;
	struct passion_int			i;
	void*						p;
#ifdef	__cplusplus
	passion_variant() = default;
	passion_variant(const passion_variant&) = default;
	explicit passion_variant(passion_object* _) : o(_) {}
	explicit passion_variant(passion_int _) : i(_) {}
	explicit passion_variant(void* _) : p(_) {}

	bool operator == (const passion_variant& _r) const noexcept { return i == _r.i; }
	bool operator != (const passion_variant& _r) const noexcept { return i != _r.i; }
#endif
};


inline extern size_t passion_variable_size_compute(passion_variable_size _data)
{
	return	_data.static_size + _data.int_count * sizeof(intmax_t);
}


#ifdef	__cplusplus
namespace psn {

using	vmobj	= passion_object;
using	vmvalue	= passion_variant;

} // namespace psn
#endif


#endif
