# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
if(CMAKE_SYSTEM_NAME MATCHES "BSD" OR CMAKE_SYSTEM_NAME MATCHES "Linux")
	if (NOT __amtrs__unix_sources)
	file(GLOB __amtrs__unix_sources	src/amtrs/unixdrv-*.cpp)
		set(__amtrs__unix_sources ${__amtrs__unix_sources} CACHE STRING "unix source" FORCE)
	endif ()
	target_sources(${libAmtrs_LIB} PUBLIC ${__amtrs__unix_sources})
endif ()
