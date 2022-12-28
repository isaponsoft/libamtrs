/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
#include <amtrs/lang/psnvm.hpp>
#include <amtrs/string/join.hpp>
#include <amtrs/system/log.hpp>

#include "lang-passion/parse-error.cxx"
#include "lang-passion/pfl.cxx"
#include "lang-passion/pflnode.cxx"
#include "lang-passion/prepro.cxx"
#include "lang-passion/pflang_serialize_psnvm.cxx"
#include "lang-passion/pflang_serialize_vnode.cxx"
#include "lang-passion/psn.cxx"
#include "lang-passion/pvlnode.cxx"
#include "lang-passion/validator.cxx"
#include "lang-passion/validator-class.cxx"
#include "lang-passion/validator-function.cxx"
#include "lang-passion/validator-node.cxx"
#include "lang-passion/vmachine.cxx"
