# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
if (Android)
	#set(LIBAMTRS_LINKER_FLAGS	"${LIBAMTRS_LINKER_FLAGS} -u Java_jp_libamtrs_ActivityInitializer_initialize")
	#set(LIBAMTRS_LINKER_FLAGS	"${LIBAMTRS_LINKER_FLAGS} -u libamtrs_initializer")

	if (NOT __amtrs__android_sources)
		file(GLOB __amtrs__android_sources	src/amtrs/amtrs/android/*.cpp)
		set(__amtrs__android_sources ${__amtrs__android_sources} CACHE STRING "android sources")
	endif()

	target_link_libraries(
		${libAmtrs_LIB}
			android camera2ndk cpufeatures log jnigraphics OpenSLES EGL GLESv3		
	)
endif ()
