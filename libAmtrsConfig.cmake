# ===================================================================================
#  libAMTRS Config
#
#	Amtrs_LIBS
#	Amtrs_INCLUDES
#
# ===================================================================================
if (NOT libAmtrs_FOUND)


enable_language(C CXX ASM)



set(libAmtrs_LIB				amtrs)
set(libAmtrs_LIBS				${libAmtrs_LIB})
set(libAmtrs_INCLUDES			${libAmtrs_DIR}/include)
set(CMAKE_MODULE_PATH			${CMAKE_MODULE_PATH} ${libAmtrs_DIR}/cmake)

#
# 依存ライブラリが格納されているディレクトリの一覧
# この中からライブラリを探します。
#
set(LIBAMTRS_DEPS_SEARCH_DIR
	${libAmtrs_DIR}/deps/
	${libAmtrs_DIR}/../
	${libAmtrs_DIR}/../libs/
	${libAmtrs_DIR}/../../
	${libAmtrs_DIR}/../../libs/
	${libAmtrs_DIR}/../../../
	${libAmtrs_DIR}/../../../libs
)


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
set(CMAKE_CXX_STANDARD	20	CACHE INTERNAL	"libamtrs c++ std" FORCE)
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")

else()
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"libamtrs c++ std" FORCE)
endif()


set(LIBAMTRS_DISABLE_MODULES_LOG		"")

# =============================================================================
# DEBUG LOG
# -----------------------------------------------------------------------------
if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DAMTRS_DEBUG_LOG_LEVEL=7)
endif()


# =============================================================================
# Windows
# -----------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message("libamtrs : Windows")
	file(GLOB AMTRS_PLATFORM_SRCS "${libAmtrs_DIR}/src/amtrs/win32-*.cpp")


# =============================================================================
# Android
# -----------------------------------------------------------------------------
elseif (ANDROID)
	message("libamtrs : Android")
	option(AMTRS_NATIVE_ACTIVITY "Use NativeActivity" OFF)

	set(LIBAMTRS_LINKER_FLAGS		"${LIBAMTRS_LINKER_FLAGS} -u Java_jp_libamtrs_ActivityInitializer_initialize")
	set(LIBAMTRS_LINKER_FLAGS		"${LIBAMTRS_LINKER_FLAGS} -u libamtrs_initializer")

	file(GLOB AMTRS_PLATFORM_SRCS "${libAmtrs_DIR}/src/amtrs/amtrs/android/*.cpp")

	# =============================================================================
	# cpu-features 
	# -----------------------------------------------------------------------------
	add_library(cpufeatures STATIC ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
	target_include_directories(${libAmtrs_LIB} PUBLIC ${ANDROID_NDK}/sources/android/cpufeatures)
	target_link_libraries(${libAmtrs_LIB} cpufeatures)
	set(libAmtrs_INCLUDES	${libAmtrs_INCLUDES} ${ANDROID_NDK}/sources/android/cpufeatures)

	# =============================================================================
	# NativeActivity 
	# -----------------------------------------------------------------------------
	#add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
	#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
	#target_link_libraries(${libAmtrs_LIB} native_app_glue)
	#target_include_directories(${libAmtrs_LIB} PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
	set(libAmtrs_INCLUDES	${libAmtrs_INCLUDES} ${ANDROID_NDK}/sources/android/native_app_glue)


	set(libAmtrs_PLATFORM_LIBS	android camera2ndk log jnigraphics OpenSLES EGL GLESv3)



# =============================================================================
# FreeBSD/Linux
# -----------------------------------------------------------------------------
elseif(CMAKE_SYSTEM_NAME MATCHES "BSD" OR CMAKE_SYSTEM_NAME MATCHES "Linux")
	message("libamtrs : Unix")

	file(GLOB AMTRS_PLATFORM_SRCS	"${libAmtrs_DIR}/src/amtrs/unixdrv-*.cpp")
	find_package(Threads REQUIRED)
	set(libAmtrs_PLATFORM_LIBS Threads::Threads ssl crypto z)


# =============================================================================
# Darwin(osx)
# -----------------------------------------------------------------------------
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	message("libamtrs : Darwin")
	file(GLOB AMTRS_PLATFORM_SRCS "${libAmtrs_DIR}/src/amtrs/amtrs/darwin-.cpp" "${libAmtrs_DIR}/src/amtrs/amtrs/unixdrv-*.cpp")
	find_package(Threads REQUIRED)
	set(libAmtrs_PLATFORM_LIBS Threads::Threads)

# =============================================================================
# Apple iOS(iPhone/iPad)
# -----------------------------------------------------------------------------
elseif(APPLE)
	message("libamtrs : iOS")

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
	add_library(${libAmtrs_LIB} STATIC ${AMTRS_SRCS})

	target_link_libraries(${libAmtrs_LIB} "System/Library/Frameworks/UIKit.framework")
	target_link_libraries(${libAmtrs_LIB} "System/Library/Frameworks/CoreGraphics.framework")
	target_link_libraries(${libAmtrs_LIB} "System/Library/Frameworks/GLKit.framework")
	target_link_libraries(${libAmtrs_LIB} "System/Library/Frameworks/OpenGLES.framework")
	target_link_libraries(${libAmtrs_LIB} "System/Library/Frameworks/QuartzCore.framework")
	target_link_libraries(${libAmtrs_LIB} "System/Library/Frameworks/StoreKit.framework")
	target_link_libraries(${libAmtrs_LIB} "System//Library/Frameworks/OpenAL.framework")
	target_link_libraries(${libAmtrs_LIB} "System//Library/Frameworks/UserNotifications.framework")
elseif (POSIX)
	message("MODE POSIX")
	add_library(
		${libAmtrs_LIB} STATIC src/amtrs/posix/dummy.cpp
	)
else()
endif()




# =============================================================================
# Module bases
# -----------------------------------------------------------------------------
message("libamtrs : Enable  APPLICATION;LOG")
set(AMTRS_SRCS	${AMTRS_SRCS}
		"${libAmtrs_DIR}/src/amtrs/allocator.c"
		"${libAmtrs_DIR}/src/amtrs/logging.cpp"
		"${libAmtrs_DIR}/src/amtrs/stringbuf.c"
		"${libAmtrs_DIR}/src/amtrs/graphics-imagebuff.cpp"
)


# =============================================================================
# Module CAMERA
# -----------------------------------------------------------------------------
if (AMTRS_CAMERA_ENABLE)
	message("libamtrs : Enable  CAMERA")
	add_definitions(-DAMTRS_CAMERA_ENABLE=1)
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/camera.cpp")
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "CAMERA")
endif ()


# =============================================================================
# Module COMMONMARK
# https://github.com/github/cmark-gfm
# https://github.com/commonmark/cmark
# -----------------------------------------------------------------------------
if (AMTRS_COMMONMARK_ENABLE)
	find_path(AMTRS_CMARK_DIR src/cmark-gfm.h
		PATHS			${LIBAMTRS_DEPS_SEARCH_DIR}
		PATH_SUFFIXES 	cmark
		NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
	message("libamtrs : Enable  COMMONMARK")
	add_subdirectory(${AMTRS_CMARK_DIR} deps-cmark)
	add_definitions(-DAMTRS_COMMONMARK_ENABLE=1)
	add_definitions(-DAMTRS_COMMONMARK_GFM_ENABLE=1)
	set(libAmtrs_INCLUDES	"${libAmtrs_INCLUDES}" "${CMAKE_CURRENT_BINARY_DIR}/deps-cmark/src" "${CMAKE_CURRENT_BINARY_DIR}/deps-cmark/extensions" "${AMTRS_CMARK_DIR}/src" "${AMTRS_CMARK_DIR}/extensions")
	set(libAmtrs_LIBS		"${libAmtrs_LIBS}" libcmark-gfm-extensions_static libcmark-gfm_static)
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/text-markdown.cpp")
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "COMMONMARK")
endif ()



# =============================================================================
# Module FONT
# -----------------------------------------------------------------------------
if (AMTRS_FONT_ENABLE)
	message("libamtrs : Enable  FONT")
	add_definitions(-DAMTRS_FONT_ENABLE=1)
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/font.cpp")
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "FONT")
endif ()



# =============================================================================
# Module VULKAN
# -----------------------------------------------------------------------------
if (AMTRS_VULKAN_ENABLE)

	find_path(VULKAN_INCLUDE_DIR NAMES vulklan/vulklan.h HINTS "")
	if (VULKAN_INCLUDE_DIR_FOUND)
	elseif (NOT "$ENV{VULKAN_SDK}" STREQUAL "")
		set(VULKAN_INCLUDE_DIR	"$ENV{VULKAN_SDK}/include")
	elseif (NOT "$ENV{VK_SDK_PATH}" STREQUAL "")
		set(VULKAN_INCLUDE_DIR	"$ENV{VK_SDK_PATH}/include")
	else ()
		message(FATAL_ERROR "Vulkan not found.") 
	endif ()

	message("libamtrs : Enable  VULKAN, ${VULKAN_INCLUDE_DIR}")
	add_definitions(-DAMTRS_VULKAN_USE=1)
	set(libAmtrs_INCLUDES	"${libAmtrs_INCLUDES}" "${VULKAN_INCLUDE_DIR}")
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/g3d-vulkan.cpp")


	set(AMTRS_G3D_ENABLE 1)
	add_definitions(-DAMTRS_VULKAN_ENABLE=1)

else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "VULKAN")
endif ()



# =============================================================================
# Module G3D
# -----------------------------------------------------------------------------
if(AMTRS_G3D_ENABLE)
	message("libamtrs : Enable  G3D")
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/g3d.cpp")
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/g3d-basicshader.cpp")
	add_definitions(-DAMTRS_G3D_ENABLE=1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "G3D")
endif ()




# =============================================================================
# Module OGG(Sound)
# -----------------------------------------------------------------------------
if(AMTRS_OGG_ENABLE OR AMTRS_SOUND_FORMAT_ALL_ENABLE)
	find_path(AMTRS_OGG_DIR include/ogg
		PATHS			${LIBAMTRS_DEPS_SEARCH_DIR}
		PATH_SUFFIXES 	libogg libogg-1.3.4
		NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

	set(OGG_DIR	${AMTRS_OGG_DIR})
	find_package(OGG QUIET)
	if(OGG_FOND)
		message("OGG : Found")
		target_include_directories(${libAmtrs_LIB} PUBLIC ${OGG_INCLUDES})
		target_link_libraries(${libAmtrs_LIB} ${OGG_LIBS})
		set(libAmtrs_INCLUDES		${libAmtrs_INCLUDES} ${OGG_INCLUDES})
	else()
		set(OGG_DIR	${AMTRS_OGG_DIR})
		message("OGG : add_subdirectory(${OGG_DIR})")
		set(BUILD_TESTING	OFF	CACHE INTERNAL "" FORCE)
		set(INSTALL_DOCS	OFF	CACHE INTERNAL "" FORCE)
		add_subdirectory(${OGG_DIR} EXCLUDE_FROM_ALL build-ogg)
		set(OGG_LIBRARIES			ogg)
		set(OGG_INCLUDE_DIRS		${CMAKE_CURRENT_BINARY_DIR}/build-ogg/include ${OGG_DIR}/include)

		#target_include_directories(${libAmtrs_LIB} PUBLIC ${OGG_INCLUDE_DIRS})
		#target_link_libraries(${libAmtrs_LIB} ${OGG_LIBRARIES})
		set(libAmtrs_INCLUDES		${libAmtrs_INCLUDES} ${OGG_INCLUDE_DIRS})
		set(libAmtrs_LIBS			${libAmtrs_LIBS} ${OGG_LIBRARIES})
	endif()



	find_path(AMTRS_VORBIS_DIR include/vorbis
		PATHS			${LIBAMTRS_DEPS_SEARCH_DIR}
		PATH_SUFFIXES 	libvorbis libvorbis-1.3.6
		NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

	set(VORBIS_DIR	${AMTRS_VORBIS_DIR})
	find_package(VORBIS QUIET)
	if(VORBIS_FOND)
		message("VORBIS : Found")
	#	target_include_directories(${libAmtrs_LIB} PUBLIC ${VORBIS_INCLUDES})
	#	target_link_libraries(${libAmtrs_LIB} ${VORBIS_LIBS})
		set(libAmtrs_INCLUDES		${libAmtrs_INCLUDES} ${VORBIS_INCLUDES})
		set(libAmtrs_LIBS			${libAmtrs_LIBS} ${VORBIS_LIBS})
	else()
		set(VORBIS_DIR	${AMTRS_VORBIS_DIR})
		message("VORBIS : add_subdirectory(${VORBIS_DIR})")
		set(BUILD_TESTING	OFF	CACHE INTERNAL "" FORCE)
		set(INSTALL_DOCS	OFF	CACHE INTERNAL "" FORCE)
		add_subdirectory(${VORBIS_DIR} EXCLUDE_FROM_ALL build-vorbis)

	#	target_include_directories(
	#		${libAmtrs_LIB} PUBLIC
	#		${CMAKE_CURRENT_BINARY_DIR}/build-vorbis/include
	#		${VORBIS_DIR}/include
	#	)
	#	target_link_libraries(${libAmtrs_LIB} vorbis vorbisfile)
		set(libAmtrs_INCLUDES		${libAmtrs_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}/build-vorbis/include ${VORBIS_DIR}/include)
		set(libAmtrs_LIBS			${libAmtrs_LIBS} vorbis vorbisfile)
	endif()
	message("libamtrs : Enable  OGG ${OGG_INCLUDES}")
	add_definitions(-DAMTRS_OGG_ENABLE=1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "OGG")
endif()



# =============================================================================
# Module OPENCV
# -----------------------------------------------------------------------------
if (AMTRS_OPENCV_ENABLE)
	if(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set(OpenCV_DIR "${CMAKE_CURRENT_LIST_DIR}/../libs/opencv-win/build")
		find_package(OpenCV REQUIRED)
		if(OpenCV_FOUND)
			target_include_directories(${libAmtrs_LIB} PUBLIC ${OpenCV_INCLUDE_DIRS})
			target_link_libraries(${libAmtrs_LIB} ${OpenCV_LIBS})
		endif()
	endif()

	if (ANDROID)
		find_path(AMTRS_OpenCV_DIR sdk/native/jni/include/opencv2
			PATHS			${libAmtrs_DIR}/../ ${libAmtrs_DIR}/../../ ${libAmtrs_DIR}/../../../
			PATH_SUFFIXES 	OpenCV-android-sdk opencv2/android
			NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)



		set(OpenCV_DIR "${AMTRS_OpenCV_DIR}/sdk/native/jni")
		find_package(OpenCV REQUIRED)
		if(OpenCV_FOUND)
			target_include_directories(${libAmtrs_LIB} PUBLIC ${OpenCV_INCLUDE_DIRS})
			target_link_libraries(${libAmtrs_LIB} ${OpenCV_LIBS})
			set(libAmtrs_INCLUDES	${libAmtrs_INCLUDES} ${OpenCV_INCLUDE_DIRS})
		endif()
	endif()

	if (APPLE)
		target_link_libraries(${libAmtrs_LIB} "${CMAKE_CURRENT_LIST_DIR}/../libs/opencv2.framework")
	endif ()

	message("libamtrs : Enable  OPENCV")
	add_definitions(-DAMTRS_OPENCV_ENABLE=1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "OPENCV")
endif ()


# =============================================================================
# Module JPEG
# -----------------------------------------------------------------------------
if(AMTRS_JPEG_ENABLE OR AMTRS_IMAGE_FORMAT_ALL_ENABLE)
	# https://libjpeg-turbo.org/
	# https://github.com/libjpeg-turbo/libjpeg-turbo/releases
	if (NOT AMTRS_LIBJPEG_PATH)
		set(AMTRS_LIBJPEG_PATH "../libjpeg-turbo")
	endif()


	# =============================================================================
	# libjpeg-turbo
	# -----------------------------------------------------------------------------
	find_package(JPEG QUIET)
	if(JPEG_FOUND)
		target_include_directories(${libAmtrs_LIB} PUBLIC ${JPEG_INCLUDE_DIRS})
		target_link_libraries(${libAmtrs_LIB} ${JPEG_LIBRARIES})
	else()
		option(ENABLE_SHARED "Build shared libraries" FALSE)
		option(WITH_JAVA     "Build Java wrapper"     FALSE)
		message("AMTRS_LIBJPEG_PATH : ${AMTRS_LIBJPEG_PATH}")
		add_subdirectory(${AMTRS_LIBJPEG_PATH} EXCLUDE_FROM_ALL  build-libjpeg-turbo)
		target_include_directories(${libAmtrs_LIB} PUBLIC ${AMTRS_LIBJPEG_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build-libjpeg-turbo)
		target_link_libraries(${libAmtrs_LIB} turbojpeg-static)
	endif()

	message("libamtrs : Enable  JPEG")
	add_definitions(-DAMTRS_JPEG_ENABLE=1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "JPEG")
endif()


# =============================================================================
# Module PNG
# -----------------------------------------------------------------------------
if(AMTRS_PNG_ENABLE OR AMTRS_IMAGE_FORMAT_ALL_ENABLE)
	# http://www.libpng.org/
	# git://git.code.sf.net/p/libpng/code
	if (NOT AMTRS_LIBPNG_PATH)
		set(AMTRS_LIBPNG_PATH "../libpng")
	endif()

	# =============================================================================
	# libpng
	# -----------------------------------------------------------------------------
	find_package(PNG QUIET)
	if(PNG_FOUND)
		target_include_directories(amtrs PUBLIC ${PNG_INCLUDE_DIRS})
		target_link_libraries(amtrs ${PNG_LIBRARIES})
	else()
		option(PNG_SHARED "Build shared lib" OFF)
		option(PNG_STATIC "Build static lib" ON)
		option(PNG_TESTS  "Build libpng tests" OFF)
		add_subdirectory(${AMTRS_LIBPNG_PATH} EXCLUDE_FROM_ALL  build-libpng)
		target_include_directories(amtrs PUBLIC ${AMTRS_LIBPNG_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build-libpng)
		target_link_libraries(amtrs png_static)
	endif()

	message("libamtrs : Enable  PNG")
	add_definitions(-DAMTRS_PNG_ENABLE=1)
	set(AMTRS_ZLIB_ENABLE 1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "PNG")
endif()





# =============================================================================
# Module ZLIB
# -----------------------------------------------------------------------------
if(AMTRS_ZLIB_ENABLE)
	find_package(ZLIB QUIET)
	if(NOT ZLIB_FOUND)
		set(PNG_BUILD_ZLIB		ON	CACHE BOOL "libpng zlib build" FORCE)
		set(SKIP_INSTALL_ALL	OFF	CACHE BOOL "zlib skip install" FORCE)

		find_path(AMTRS_ZLIB_PATH zlib.h
			PATHS			${LIBAMTRS_DEPS_SEARCH_DIR}
			PATH_SUFFIXES 	zlib
			NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

		add_subdirectory(${AMTRS_ZLIB_PATH} EXCLUDE_FROM_ALL  build-zlib)
		set(ZLIB_INCLUDE_DIR
			${AMTRS_ZLIB_PATH} ${CMAKE_CURRENT_BINARY_DIR}/build-zlib
			CACHE STRING "zlib include" FORCE)
		set(ZLIB_LIBRARY		zlibstatic)

		set(libAmtrs_INCLUDES	"${libAmtrs_INCLUDES}" "${AMTRS_ZLIB_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/build-zlib")
		set(libAmtrs_LIBS		"${libAmtrs_LIBS}" zlibstatic)
	endif()
	message("libamtrs : Enable  ZLIB, ${CMAKE_CURRENT_BINARY_DIR}")
	add_definitions(-DAMTRS_ZLIB_ENABLE=1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "ZLIB")
endif()


# =============================================================================
# Module bases
# -----------------------------------------------------------------------------
if(AMTRS_APPLICATION_ENABLE)
	message("libamtrs : Enable  APPLICATION")
	set(AMTRS_SRCS	${AMTRS_SRCS} "${libAmtrs_DIR}/src/amtrs/application.cpp")
	add_definitions(-DAMTRS_APPLICATION_ENABLE=1)
else ()
	set(LIBAMTRS_DISABLE_MODULES_LOG	${LIBAMTRS_DISABLE_MODULES_LOG} "APPLICATION")
endif ()



message("libamtrs : Disable ${LIBAMTRS_DISABLE_MODULES_LOG}")


add_library(${libAmtrs_LIB} STATIC ${AMTRS_SRCS} ${AMTRS_PLATFORM_SRCS})
set(libAmtrs_LIBS ${libAmtrs_LIBS} ${libAmtrs_PLATFORM_LIBS})

message("libAmtrs_LIBS : ${libAmtrs_LIBS}")


include_directories(${libAmtrs_INCLUDES})


endif ()	# (NOT libAmtrs_FOUND)
