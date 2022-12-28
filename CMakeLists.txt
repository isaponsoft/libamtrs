# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
# ======================================================================
# g3d library:
#	$ENV{VULKAN_SDK}
#
# ----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10.0)
project(amtrs)
include(GNUInstallDirs)
set(CMAKE_MODULE_PATH	"${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(AMTRS_SRCES
	src/amtrs/core.cpp
	src/amtrs/console.cpp
	src/amtrs/net.cpp
	src/amtrs/lang-passion.cpp
	src/amtrs/system.cpp
)

if (ENABLE_GUI)
set(AMTRS_GUI_SRCES
	src/amtrs/g3d.cpp
	src/amtrs/gui.cpp
)
else()
set(AMTRS_GUI)
endif ()

#set(PSNVM_SRCES
#	src/psnvm/core.cpp
#	src/psnvm/lang.cpp
#	src/psnvm/term.cpp
#)

set(INCLUDE_DIRS	${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIBRARIES		${CMAKE_CURRENT_SOURCE_DIR}/include)

# $ENV{VULKAN_SDK}
find_package(Vulkan)
if (Vulkan_FOUND)
	set(INCLUDE_DIRS	${CMAKE_CURRENT_SOURCE_DIR}/include ${Vulkan_INCLUDE_DIRS})
endif()


if(MSVC)
add_compile_options("$<$<CONFIG:Debug>:/DDEBUG /D_DEBUG>")
add_compile_options("/DAMTRS_EXPORT_DLL")
else()
add_compile_options("$<$<CONFIG:Debug>:-DDEBUG -D_DEBUG>")
endif()

# amtrs static library
add_library(amtrs_static STATIC ${AMTRS_SRCES} ${AMTRS_GUI_SRCES})
set_property(TARGET amtrs_static PROPERTY CXX_STANDARD 17)
target_include_directories(amtrs_static BEFORE PUBLIC ${INCLUDE_DIRS})
if(NOT MSVC)
	target_link_libraries(amtrs_static dl ${LIBRARIES})
endif()
set_target_properties(amtrs_static PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/amtrs)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/amtrs TYPE INCLUDE)


# psnvm static library
#add_library(psnvm_static STATIC ${PSNVM_SRCES})
#set_property(TARGET psnvm_static PROPERTY CXX_STANDARD 17)
#target_include_directories(psnvm_static BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_link_libraries(psnvm_static amtrs_static)


# BUILD: psnvm
if (NOT DISABLE_PSNVM)
	add_executable(psnvm src/cmd/psnvm.cpp ${PSNVM_SRCES})
	target_include_directories(psnvm BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	set_property(TARGET psnvm PROPERTY CXX_STANDARD 17)
	target_link_libraries(psnvm amtrs_static)

	install(TARGETS psnvm)
endif()


# BUILD: slsh
#if (NOT DISABLE_SLSH)
#	add_executable(slsh src/cmd/slsh.cpp ${PSNVM_SRCES})
#	target_include_directories(slsh BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#	set_property(TARGET slsh PROPERTY CXX_STANDARD 17)
#	target_link_libraries(slsh psnvm_static amtrs_static)
#
#	install(TARGETS slsh)
#endif()


# BUILD: psnvm modules
#if (NOT DISABLE_PSNVM_MODULE)
#	add_library(console SHARED src/psnvm/modules/mod-console.cpp)
#	target_include_directories(console BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#	set_property(TARGET console PROPERTY CXX_STANDARD 17)
#	SET_TARGET_PROPERTIES(console PROPERTIES PREFIX "")
#	target_link_libraries(console amtrs_static)
#	install(TARGETS console)
#
#	add_library(compiler SHARED src/psnvm/modules/mod-compiler.cpp)
#	target_include_directories(compiler BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#	set_property(TARGET compiler PROPERTY CXX_STANDARD 17)
#	SET_TARGET_PROPERTIES(compiler PROPERTIES PREFIX "")
#	target_link_libraries(compiler amtrs_static psnvm_static)
#	install(TARGETS compiler)
#
#	add_library(process SHARED src/psnvm/modules/mod-process.cpp)
#	target_include_directories(process BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#	set_property(TARGET process PROPERTY CXX_STANDARD 17)
#	SET_TARGET_PROPERTIES(process PROPERTIES PREFIX "")
#	target_link_libraries(process amtrs_static psnvm_static)
#	install(TARGETS process)
#endif()


if (NOT DISABLE_BIN2H)
	add_executable(bin2h src/cmd/bin2h.cpp)
	set_property(TARGET bin2h PROPERTY CXX_STANDARD 17)
	target_link_libraries(bin2h amtrs_static)
endif()

if (NOT DISABLE_LSON)
	add_executable(lson src/cmd/lson.cpp)
	set_property(TARGET lson PROPERTY CXX_STANDARD 17)
	target_link_libraries(lson amtrs_static)
endif()

if (NOT DISABLE_STH)
	add_executable(sth src/cmd/sth.cpp)
	set_property(TARGET sth PROPERTY CXX_STANDARD 17)
	target_link_libraries(sth amtrs_static)
endif()


# BUILD: test
if (ENABLE_TEST)
	add_subdirectory(tests)
endif()

if (ENABLE_EXAMPLE)
	add_subdirectory(examples/echosrv)
	add_subdirectory(examples/g3d)
endif()

