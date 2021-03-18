# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	if (NOT __amtrs__platform_sources)
		file(GLOB __amtrs__platform_sources "src/amtrs/darwin-*.cpp" "src/amtrs/amtrs/unixdrv-*.cpp")
		set(__amtrs__platform_sources ${__amtrs__platform_sources} CACHE STRING "platform source" FORCE)
	endif ()
	target_sources(${AMTRS_LIBRARY} PUBLIC ${__amtrs__platform_sources})
endif ()
