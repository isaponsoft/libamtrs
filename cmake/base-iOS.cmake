# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
if(APPLE)
	set(CMAKE_MACOSX_BUNDLE				YES				CACHE INTERNAL "" FORCE)
	set(CMAKE_SYSTEM_NAME				iOS				CACHE INTERNAL "" FORCE)
	set(CMAKE_OSX_SYSROOT				"iphoneos"		CACHE INTERNAL "" FORCE)
	set(CMAKE_TRY_COMPILE_TARGET_TYPE	STATIC_LIBRARY	CACHE INTERNAL "" FORCE)	# require to configure.


	if (NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
	set(CMAKE_OSX_DEPLOYMENT_TARGET		"11.0"			CACHE STRING   "" FORCE)
	endif()


	set(CMAKE_CXX_FLAGS
		"$(OTHER_CFLAGS) ${CMAKE_CXX_FLAGS} -x objective-c++"
		CACHE STRING "" FORCE)
	add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

	file(GLOB AMTRS_SRCS
			"${libAmtrs_DIR}/src/amtrs/ios/*.cpp"
			"${libAmtrs_DIR}/src/amtrs/opengl/*.cpp"
	)
	add_library(${AMTRS_LIBRARY} STATIC ${AMTRS_SRCS})

	target_link_libraries(${AMTRS_LIBRARY} "System/Library/Frameworks/UIKit.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System/Library/Frameworks/CoreGraphics.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System/Library/Frameworks/GLKit.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System/Library/Frameworks/OpenGLES.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System/Library/Frameworks/QuartzCore.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System/Library/Frameworks/StoreKit.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System//Library/Frameworks/OpenAL.framework")
	target_link_libraries(${AMTRS_LIBRARY} "System//Library/Frameworks/UserNotifications.framework")
endif()
