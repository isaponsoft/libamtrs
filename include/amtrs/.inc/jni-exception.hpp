/* Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__java__exception__hpp
#define	__libamtrs__java__exception__hpp
AMTRS_JAVA_NAMESPACE_BEGIN

inline extern bool has_exception()
{
	return	java::get_jnienv()->ExceptionCheck();
}

inline extern void clear_exception()
{
	java::get_jnienv()->ExceptionClear();
}


AMTRS_JAVA_NAMESPACE_END
#endif
