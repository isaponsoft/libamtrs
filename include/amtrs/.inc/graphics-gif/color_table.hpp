/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__graphics__gif__color_table__hpp
#define	__libamtrs__graphics__gif__color_table__hpp
#include "header.hpp"
AMTRS_GRAPHICS_GIF_NAMESPACE_BEGIN



template<class Containor, class In>
auto read_color_table(In&& _in, size_t _tableSize)
{
	if (_tableSize == 0)
	{
		return	Containor();
	}
	Containor	colortable(_tableSize);
	for (auto& c : colortable)
	{
		io::read(c, std::forward<In>(_in));
	}
	return	colortable;
}



AMTRS_IO_NAMESPACE_END
#endif
