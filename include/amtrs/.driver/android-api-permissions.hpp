/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__os__android__permissions__hpp
#define	__libamtrs__os__android__permissions__hpp
#include "../java/java/lang/String.hpp"
#include "../java/android/Manifest.hpp"
#include "android-api-def.hpp"
AMTRS_OS_ANDROID_NAMESPACE_BEGIN


// ==================================================================
//! ユーザーにパーミッションの許可を問い合わせます。
// ------------------------------------------------------------------
//! 結果はコールバックで返されます。
// ------------------------------------------------------------------
void permission(std::initializer_list<java::jobj<java::classes::java::lang::String>> _perms, std::function<void(bool)> _callback);


AMTRS_OS_ANDROID_NAMESPACE_END
#endif
