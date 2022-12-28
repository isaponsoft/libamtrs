/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__android__input_keyboard__hpp
#define	__libamtrs__driver__android__input_keyboard__hpp
#include <android_native_app_glue.h>
AMTRS_NAMESPACE_BEGIN

key_code get_key_code(int _androidKeyCode);

AMTRS_NAMESPACE_END
#endif
