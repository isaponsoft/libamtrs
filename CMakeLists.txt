# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0091 NEW)
project(amtrs)

set(CMAKE_MODULE_PATH	"${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(amtrs-utils)


amtrs_make_libname(AMTRS_LIBRARY		amtrs _static d)
set(AMTRS_INCLUDE_DIR					"${CMAKE_CURRENT_SOURCE_DIR}/include")


option(AMTRS_ALL_ENABLE					"Enable all module." ON)
option(AMTRS_USE_MSVC_RUNTIME_STATIC	"Sugger option. Set CMAKE_MSVC_RUNTIME_LIBRARY = MultiThreaded$<$<CONFIG:Debug>:Debug>." OFF)
option(AMTRS_USE_STATIC_LIBRARY			"Link library priority static." ON)
option(AMTRS_NO_COPYLEFT				"No use copyleft libraries." ON)

if (AMTRS_USE_STATIC_LIBRARY)
	if (MSVC)
	else()
	set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
	endif()
endif()

if(AMTRS_USE_MSVC_RUNTIME_STATIC)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


if(NOT AMTRS_DEPENENCY_INSTALL_DIR)
	set(AMTRS_DEPENENCY_INSTALL_DIR	${CMAKE_BINARY_DIR})
endif()


add_library(${AMTRS_LIBRARY} STATIC)

file(GLOB _AMTRS_BASESRCS
	src/amtrs/base-*.cpp
)

file(GLOB _AMTRS_HEADERS
	include/amtrs/*.h
	include/amtrs/*.hpp
	include/amtrs/*/*.h
	include/amtrs/*/*.hpp
	include/amtrs/*/*/*.h
	include/amtrs/*/*/*.hpp
)
target_sources(${AMTRS_LIBRARY} PRIVATE ${_AMTRS_BASESRCS})
target_sources(${AMTRS_LIBRARY} PUBLIC ${_AMTRS_HEADERS})
set_target_properties(${AMTRS_LIBRARY} PROPERTIES PUBLIC_HEADER "${_AMTRS_HEADERS}")


set(AMTRS_ADD_LICENCES		AMTRS)
set(AMTRS_AMTRS_NAME		"libAmtrs")
set(AMTRS_AMTRS_LICENSE_URL	"https://raw.githubusercontent.com/isaponsoft/libamtrs/master/LICENSE")

# =============================================================================
# DEBUG LOG
# -----------------------------------------------------------------------------
if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DAMTRS_DEBUG_LOG_LEVEL=7)
endif()

set(AMTRS_DEPENENCY_INCLUDE_DIRS	"${AMTRS_INCLUDE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/include" "${AMTRS_DEPENENCY_INSTALL_DIR}/include")
set(AMTRS_LINK_LIBRARIES)


# Platform
include(base-Android)		# Google Android
include(base-Darwin)		# Darwin and Apple osx
include(base-iOS)			# Apple iOS/iPadOS
include(base-Unix)			# Unix like OS, *BSD, Linux base OS
include(base-Win32)			# Microsoft Windows WIN32 API


# Modules
amtrs_import(Zlib)			# zlib link
amtrs_import(PNG)			# libpng link
amtrs_import(JPEG)			# libjpeg-turbo link
amtrs_import(SSL)			# libressl or openssl link
amtrs_import(Archive)		# libarchive link
amtrs_import(OpenCV)		# OpenCV link
amtrs_import(OGG)			# ogg link
amtrs_import(Vorbis)		# vorbis link
amtrs_import(Cmark)			# cmark link
amtrs_import(VULKAN)		# vulkan link
amtrs_import(Threads)		# threads link


configure_file(include/amtrs/amtrs-config.hpp.in amtrs-config.hpp)



# Override c++ version flags.
if(MSVC)
set_property(TARGET ${AMTRS_LIBRARY} PROPERTY  CXX_STANDARD	20)
set(CMAKE_CXX_STANDARD	20	CACHE INTERNAL	"libamtrs c++ std" FORCE)
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
else()
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"libamtrs c++ std" FORCE)
set_property(TARGET ${AMTRS_LIBRARY} PROPERTY  CXX_STANDARD	17)
endif()




list(REMOVE_DUPLICATES AMTRS_DEPENENCY_INCLUDE_DIRS)
list(REMOVE_DUPLICATES AMTRS_LINK_LIBRARIES)

set(AMTRS_INCLUDE_DIRS	${AMTRS_INCLUDE_DIR} ${AMTRS_DEPENENCY_INCLUDE_DIRS})
set(AMTRS_LIBRARIES		${AMTRS_LIBRARY} ${AMTRS_LINK_LIBRARIES})
include_directories		(${AMTRS_LIBRARY} SYSTEM BEFORE PUBLIC ${AMTRS_INCLUDE_DIRS})
target_link_libraries	(${AMTRS_LIBRARY}                      ${AMTRS_LINK_LIBRARIES})
message("Link libraries ${AMTRS_LINK_LIBRARIES}")


# ===================================================================
# License file generate.
# -------------------------------------------------------------------
if(NOT EXISTS ${CMAKE_BINARY_DIR}/include/amtrs/amtrs-license.hpp)
	list(REMOVE_DUPLICATES AMTRS_ADD_LICENCES)
	set(LFILE		${CMAKE_BINARY_DIR}/include/amtrs/amtrs-license.hpp)
	set(LFILERAW	${CMAKE_BINARY_DIR}/include/amtrs/LICENSE.txt)
	set(LLINK)
	foreach (name ${AMTRS_ADD_LICENCES})
	message("LICENSE Tag=${name}, Name=${AMTRS_${name}_NAME}, Url=${AMTRS_${name}_LICENSE_URL}")


	file(APPEND "${LFILE}" "license ${name}(\"${AMTRS_${name}_NAME}\", R\"***(")

	if(NOT EXISTS "${CMAKE_BINARY_DIR}/LICENSE-${name}")
		file(DOWNLOAD ${AMTRS_${name}_LICENSE_URL} "${CMAKE_BINARY_DIR}/LICENSE-${name}")
	endif()
	file(READ "${CMAKE_BINARY_DIR}/LICENSE-${name}" CONTENTS)
	file(APPEND "${LFILE}" "${CONTENTS}")
	file(APPEND "${LFILERAW}" "${CONTENTS}\n\n")

	file(APPEND "${LFILE}" ")***\");\n\n\n")
	list(APPEND LLINK ${name})

	endforeach()

	file(APPEND "${LFILE}" "license* licenses[] = {")
	foreach (name ${AMTRS_ADD_LICENCES})
	file(APPEND "${LFILE}" "&${name},")
	endforeach()

	file(APPEND "${LFILE}" "nullptr};\n\n")
endif()
# -------------------------------------------------------------------


install(TARGETS			${AMTRS_LIBRARY}
		RUNTIME			DESTINATION bin
		ARCHIVE			DESTINATION lib
		LIBRARY			DESTINATION lib
		)
install(DIRECTORY include/amtrs DESTINATION include)

install(FILES ${CMAKE_BINARY_DIR}/amtrs-config.hpp DESTINATION include/amtrs)
install(FILES ${CMAKE_BINARY_DIR}/include/amtrs/amtrs-license.hpp DESTINATION include/amtrs)
install(FILES ${CMAKE_BINARY_DIR}/include/amtrs/LICENSE.txt DESTINATION include/amtrs)

add_subdirectory(src/tool-icpp)
