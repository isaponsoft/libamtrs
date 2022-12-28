/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * システムに関するライブラリ
 * 環境変数やファイルシステムへのアクセスを提供する。
 */
#include <amtrs/io/fdesc.hpp>
#include <amtrs/system/process.hpp>

#include "system/filesystem-path.cxx"
#include "system/filesystem-util.cxx"
#include "system/io-fdesc.cxx"
#include "system/system-log.cxx"
#include "system/system-process.cxx"
#include "system/system-sleep.cxx"

#include AMTRS_PLATFORM_SUBCODE(amtrs-filesystem-enumfiles)
#include AMTRS_PLATFORM_SUBCODE(amtrs-filesystem-path)
#include AMTRS_PLATFORM_SUBCODE(amtrs-io-fdesc)
#include AMTRS_PLATFORM_SUBCODE(amtrs-system)
#include AMTRS_PLATFORM_SUBCODE(amtrs-system-locale)
#include AMTRS_PLATFORM_SUBCODE(amtrs-system-log)
#include AMTRS_PLATFORM_SUBCODE(amtrs-system-dynamic_library)
#include AMTRS_PLATFORM_SUBCODE(amtrs-system-process)
