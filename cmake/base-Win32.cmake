# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	if (NOT __amtrs__windows_win32_sources)
		file(GLOB __amtrs__windows_win32_sources "src/amtrs/win32-*.cpp")
		set(__amtrs__windows_win32_sources ${__amtrs__windows_win32_sources} CACHE STRING "win32 source" FORCE)
	endif ()
	target_sources(${libAmtrs_LIB} PUBLIC ${__amtrs__windows_win32_sources})
endif ()
