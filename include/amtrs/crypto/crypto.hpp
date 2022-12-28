/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__crypto__hpp
#define	__libamtrs__crypto__hpp
#include "amtrs.hpp"

#define	AMTRS_CRYPTO_NAMESPACE_BEGIN	AMTRS_NAMESPACE_BEGIN namespace crypto {
#define	AMTRS_CRYPTO_NAMESPACE_END		} AMTRS_NAMESPACE_END
AMTRS_CRYPTO_NAMESPACE_BEGIN
AMTRS_CRYPTO_NAMESPACE_END

#include ".inc-crypto/crypto-aes.hpp"
#include ".inc-crypto/crypto-crc32.hpp"
#include ".inc-crypto/crypto-md5.hpp"
#include ".inc-crypto/crypto-sha256.hpp"
#endif
