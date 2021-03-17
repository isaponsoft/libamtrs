#
add_library(amtrs_static STATIC)


# amtrs base system
file(GLOB _AMTRS_BASES
	base-*.cpp
	${_AMTRS_ROOT_DIR}/include/amtrs/*
	${_AMTRS_ROOT_DIR}/include/amtrs/*/*
	${_AMTRS_ROOT_DIR}/include/amtrs/*/*/*
)

target_sources(amtrs_static PUBLIC ${_AMTRS_BASES})
unset(_AMTRS_BASES)
target_include_directories(amtrs_static PUBLIC ${_AMTRS_ROOT_DIR}/include)
target_link_options(amtrs_static INTERFACE "/ignore:4099")

if (CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(amtrs_static PUBLIC AMTRS_DEBUG_LOG_LEVEL=7)
endif()


# Platform
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(GLOB _SRCS win32-*.cpp)
	target_sources(amtrs_static PUBLIC ${_SRCS})
	unset(_SRCS)

elseif (ANDROID)
	file(GLOB _SRCS android-*.cpp)
	target_sources(amtrs_static PUBLIC ${_SRCS})
	target_sources(amtrs_static PUBLIC ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
	unset(_SRCS)

elseif(CMAKE_SYSTEM_NAME MATCHES "BSD" OR CMAKE_SYSTEM_NAME MATCHES "Linux")
	file(GLOB _SRCS unixdrv-*.cpp)
	target_sources(amtrs_static PUBLIC ${_SRCS})
	target_sources(amtrs_static PUBLIC ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
	unset(_SRCS)

elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	file(GLOB _SRCS unixdrv-*.cpp darwin-*.cpp)
	target_sources(amtrs_static PUBLIC ${_SRCS})
	unset(_SRCS)

elseif(APPLE)
	file(GLOB _SRCS unixdrv-*.cpp ios-*.cpp)
	target_sources(amtrs_static PUBLIC ${_SRCS})
	unset(_SRCS)
	target_link_libraries(amtrs_static "System/Library/Frameworks/UIKit.framework")
	target_link_libraries(amtrs_static "System/Library/Frameworks/CoreGraphics.framework")
	target_link_libraries(amtrs_static "System/Library/Frameworks/GLKit.framework")
	target_link_libraries(amtrs_static "System/Library/Frameworks/OpenGLES.framework")
	target_link_libraries(amtrs_static "System/Library/Frameworks/QuartzCore.framework")
	target_link_libraries(amtrs_static "System/Library/Frameworks/StoreKit.framework")
	target_link_libraries(amtrs_static "System//Library/Frameworks/OpenAL.framework")
	target_link_libraries(amtrs_static "System//Library/Frameworks/UserNotifications.framework")
endif()
target_link_libraries(amtrs_static ${AMTRS_DEPENENCY_LIBRARIES})
target_include_directories(amtrs_static PUBLIC ${AMTRS_DEPENENCY_INCLUDES})
target_include_directories(amtrs_static PUBLIC include)

set(AMTRS_DEPENENCY_LIBRARIES)
set(AMTRS_DEPENENCY_INCLUDES)
include(Zlib)
include(SSL)
include(Archive)


set(Amtrs_LIBRARIES amtrs_static)

install(TARGETS	amtrs_static
				RUNTIME DESTINATION bin
				ARCHIVE DESTINATION lib
				LIBRARY DESTINATION lib)

install(DIRECTORY ${_AMTRS_ROOT_DIR}/include/amtrs DESTINATION include)
