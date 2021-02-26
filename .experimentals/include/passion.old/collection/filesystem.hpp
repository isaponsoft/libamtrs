/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__passion__collection__filesystem__hpp
#define	__passion__collection__filesystem__hpp
#include <iostream>
#include <string>
#include <cstddef>
#include <amtrs/filesystem/@>
#include "../psnvm.hpp"
PASSION_NAMESPACE_BEGIN namespace collection {




inline void register_filesystem(vmclass_manager* _vm)
{
	auto	clazz	= _vm->get_global_scope();
	clazz->add_function(native::create_static_function(_vm, clazz->name(), "getcwd", [](passion_context& _context) -> vmstring
	{
		return	filesystem::current_path<std::string>();
	}));

	clazz->add_function(native::create_static_function(_vm, clazz->name(), "dirname", [](passion_context& _context, vmstring _filename) -> vmstring
	{
		return	vmstring(filesystem::parent_path((std::string_view)_filename));
	}));


	clazz->add_function(native::create_static_function(_vm, clazz->name(), "file_get_contents", [](passion_context& _context, vmstring _filename) -> vmstring
	{
		return	filesystem::file_get_contents<std::string>(_filename.c_str());
	}));


	clazz->add_function(native::create_static_function(_vm, clazz->name(), "file_put_contents", [](passion_context& _context, vmstring _filename, vmstring _data) -> vmint_t
	{
		filesystem::file_put_contents<std::string>(_filename.c_str(), _data);
		return	0;
	}));


	clazz->add_function(native::create_static_function(_vm, clazz->name(), "file_exists", [](passion_context& _context, vmstring _path) -> vmint_t
	{
		return	filesystem::exists(_path.c_str())
				? 1
				: 0;
	}));


	clazz->add_function(native::create_static_function(_vm, clazz->name(), "is_directory", [](passion_context& _context, vmstring _path) -> vmint_t
	{
		return	filesystem::is_directory(_path.c_str())
				? 1
				: 0;
	}));


	clazz->add_function(native::create_static_function(_vm, clazz->name(), "file_size", [](passion_context& _context, vmstring _path) -> vmint_t
	{
		return	static_cast<vmint_t>(filesystem::file_size(_path.c_str()));
	}));

	clazz->build();
}

} PASSION_NAMESPACE_END
#endif
