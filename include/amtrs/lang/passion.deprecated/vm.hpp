/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__lang__passion__vm__hpp
#define	__libamtrs__lang__passion__vm__hpp
#include "../../memory/stackbuff.hpp"
#include "../../string/sourcecode.hpp"

// std c/c++ libraries.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <tuple>
#if		defined(DEBUG) || defined(_DEBUG)
#include <type_traits>
#endif
//

#include "namespace.hxx"
#include "vm-datatype.hxx"
#include "vm-bytecode.hxx"
#include "vm-functions.hxx"
#include "vm-vmclass.hxx"
#include "vm-vmstate.hxx"
PSNVM_NAMESPACE_BEGIN

// VMの実行関数(リファレンスモデル)
PSNVM_API function_result vmexec_reference(vmstate*, function*);

// VMの実行関数(高速モデル)
PSNVM_API function_result vmexec_fast(vmstate*, function*);

// VMの実行関数(セキュアモデル)
PSNVM_API function_result vmexec_safety(vmstate*, function*);


// 指定する関数をVMを実行し int 型の戻り値を返す
vmint vmrun(vmstate& _vm, function* _f);

// 指定する関数名の関数を検索してVMを実行し int 型の戻り値を返す
vmint vmrun(vmstate& _vm, char const* _entry);


PSNVM_NAMESPACE_END
#endif
