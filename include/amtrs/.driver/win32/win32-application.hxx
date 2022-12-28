/* Copyright (c) 2022, isaponsoft (Isao Shibuya) All rights reserved. *
 * Use of this source code is governed by a BSD-style  license that   *
 * can be found in the LICENSE file.                                  */
#ifndef	__libamtrs__driver__win32__application__hpp
#define	__libamtrs__driver__win32__application__hpp
#include <windows.h>

AMTRS_OS_NAMESPACE_BEGIN
int win32_main(int _argc, char** _argv);
int win32_wmain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
AMTRS_OS_NAMESPACE_END


#define	AMTRS_APPLICATIN_ENTRY(_listener_classname)\
	int main(int _argc, char** _argv)\
	{\
		return	AMTRS_NAMESPACE::os::win32_main(_argc, _argv);\
	}
#endif
