/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__passion__assembler__hpp
#define	__libamtrs__passion__assembler__hpp
#include "core-error.hpp"
PSNVM_NAMESPACE_BEGIN
struct		vmstate;
struct		function;
struct		function_bc;
class		vmclassmgr;
enum class	opecode;
PSNVM_NAMESPACE_END
PSNVM_LANG_NAMESPACE_BEGIN

PSNVM_API void dump(PSNVM_NAMESPACE::function_bc* _function);
PSNVM_API char const* to_string(PSNVM_NAMESPACE::opecode _code);

PSNVM_LANG_NAMESPACE_END
#endif
