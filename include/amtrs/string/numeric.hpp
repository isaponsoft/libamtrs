/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__string__numeric__hpp
#define	__libamtrs__string__numeric__hpp
#include "../amtrs.hpp"
AMTRS_STRING_NAMESPACE_BEGIN

/*
 * バッファに整数値を書き込みます。
 * size_t には書き込んだサイズ。バッファのサイズが不足していた場合は不定です。
 */
size_t write_int(char* _buffer, size_t _bufferSize, intmax_t _num, int _base = 10) noexcept;




AMTRS_STRING_NAMESPACE_END
#endif
