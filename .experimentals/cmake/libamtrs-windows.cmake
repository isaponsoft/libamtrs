if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
	file(GLOB _AMTRS_SRCS	"${libAmtrs_DIR}/src/*.cpp" "${libAmtrs_DIR}/src/uwp/*.cpp")
else()
	file(GLOB _AMTRS_SRCS	"${libAmtrs_DIR}/src/*.cpp" "${libAmtrs_DIR}/src/win32/*.cpp")
endif()
set(AMTRS_SRCS	${_AMTRS_SRCS})

# =============================================================================
# find_packageでzlibが見つからなければ AMTRS_ZLIB_PATH を用いる。
# -----------------------------------------------------------------------------
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND)

	set(PNG_BUILD_ZLIB		ON	CACHE BOOL "libpng zlib build" FORCE)
	set(SKIP_INSTALL_ALL	OFF	CACHE BOOL "zlib skip install" FORCE)

	find_path(AMTRS_ZLIB_PATH zlib.h
		PATHS			${libAmtrs_DIR}/../ ${libAmtrs_DIR}/../../ ${libAmtrs_DIR}/../../../
		PATH_SUFFIXES 	zlib
		NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

	add_subdirectory(${AMTRS_ZLIB_PATH} EXCLUDE_FROM_ALL  build-zlib)
	set(ZLIB_INCLUDE_DIR
		${AMTRS_ZLIB_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build-zlib
		CACHE STRING "zlib include" FORCE)
	set(ZLIB_LIBRARY		zlibstatic)
	include_directories(${AMTRS_ZLIB_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build-zlib)
	#target_link_libraries(amtrs zlibstatic)
	set(libAmtrs_LIBS		${libAmtrs_LIBS} zlibstatic)

endif()


set(CMAKE_C_FLAGS	"${CMAKE_C_FLAGS} /D_CRT_SECURE_NO_WARNINGS"   CACHE STRING "amtrs c flags"   FORCE)
set(CMAKE_CXX_FLAGS	"${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS" CACHE STRING "amtrs c++ flags" FORCE)
include("${libAmtrs_DIR}/cmake/lib-image-jpeg.cmake")
include("${libAmtrs_DIR}/cmake/lib-image-png.cmake")
include("${libAmtrs_DIR}/cmake/lib-sound-ogg.cmake")



# =============================================================================
# OpenGL/OpenGL ES
# -----------------------------------------------------------------------------
if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
	find_program(NUGET_EXE nuget.exe PATHS . NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
	if(NOT NUGET_EXE)
		message(FATAL_ERROR "nuget.exe not found.")
	endif()

	exec_program(${NUGET_EXE} ARGS install
		"ANGLE.WindowsStore"
		-ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/nuget_packages)
	#target_link_libraries(amtrs  ${UWP_ANGLE_GLES_TARGETS_DIR}/ANGLE.WindowsStore.targets)
else()
	# https://github.com/KhronosGroup/EGL-Registry
	# https://github.com/KhronosGroup/OpenGL-Registry
	include_directories(../EGL-Registry/api ../OpenGL-Registry/api)
endif()




add_library(
	${libAmtrs_LIB} STATIC
	${AMTRS_SRCS}
)


#find_program(NUGET_EXE nuget.exe PATHS . NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

