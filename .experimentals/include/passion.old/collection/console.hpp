/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__collection__console__hpp
#define	__passion__collection__console__hpp
#include <stdlib.h>
#include <sstream>
#include <string>
#include <amtrs/system/@>
#include "../psnvm.hpp"
PASSION_NAMESPACE_BEGIN namespace collection {




inline void register_console(vmclass_manager* _vm, const char* _vm_path)
{
	auto	clazz	= _vm->get_global_scope();

	clazz->add_function(native::create_static_function(_vm, clazz->name(), "printf", [](passion_context& _context, vmstring _format)
	{
		::fwrite(_format.c_str(), 1, (unsigned int)_format.size(), stdout);
	}));

	clazz->add_function(native::create_static_function(_vm, clazz->name(), "printf", [](passion_context& _context, vmint_t _n)
	{
		char	buff[100];
		auto	len	= ::snprintf(buff, sizeof(buff), "%zd", (size_t)_n);
		::fwrite(buff, 1, len, stdout);
	}));

	clazz->add_function(native::create_static_function(_vm, clazz->name(), "vmpath", [_vm_path](passion_context& _context) -> vmstring
	{
		return	_vm_path;
	}));
/*
	clazz->add_function(native::create_vfunc("gets", []() -> vmstring
	{
		std::string	text;
		std::getline(std::cin, text);
		return	text;
	}));



	clazz->add_function(native::create_vfunc("getenv", &getenv<vmstring>));
*/
	clazz->build();
}


} PASSION_NAMESPACE_END

#endif
