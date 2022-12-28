/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
/*
 * 汎用ライブラリ
 * OSや環境問わずに使用できる。
 */
#include <amtrs/amtrs.hpp>
#include <amtrs/string/sstring.hpp>
#include <amtrs/string/convert.hpp>
#include <amtrs/string/wildcard.hpp>

#include "core/errorval.cxx"
#include "core/io-reader.cxx"
#include "core/io-writer.cxx"
#include "core/string-format.cxx"
#include "core/string-multibyte.cxx"
#include "core/string-numeric.cxx"
#include "core/string-sstring.cxx"
#include "core/string-sourcecode.cxx"
#include "core/string-text.cxx"
#include "core/string-wildcard.cxx"
#include "core/string-trnstext.cxx"
#include "core/tests.cxx"


#include "graphics/image-bmp.cxx"


#include AMTRS_PLATFORM_SUBCODE(amtrs-core-error)
#include AMTRS_PLATFORM_SUBCODE(amtrs-core-string-convert)
