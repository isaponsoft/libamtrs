/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__sourcecode__hpp
#define	__libamtrs__passion__sourcecode__hpp
#include "def.hpp"
#include "string.hpp"
PASSION_NAMESPACE_BEGIN


struct	vmsourcecode
{
	using	value_type			= vmstring::value_type;
	using	pointer				= vmstring::pointer;
	using	const_pointer		= vmstring::const_pointer;

	vmstring		filename;
	vmstring		source;


	static vmsourcecode load(std::string const& _filename);


	vmsourcecode() = default;
	vmsourcecode(vmsourcecode const&) = default;
	vmsourcecode(vmsourcecode&&) = default;
	vmsourcecode& operator = (vmsourcecode const&) = default;
	vmsourcecode& operator = (vmsourcecode&&) = default;


	auto begin() noexcept { return source.begin(); }
	auto end() noexcept { return source.end(); }
	auto begin() const noexcept { return source.begin(); }
	auto end() const noexcept { return source.end(); }
};


PASSION_NAMESPACE_END
#endif
