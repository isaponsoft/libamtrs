cmake_minimum_required(VERSION 3.10.2)
# Windows runtime library
if(POLICY CMP0091)
	cmake_policy(SET CMP0091 NEW)
endif()
project(md5)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	# CMP0091
	set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


add_executable(
	md5
	md5.cpp
)


target_include_directories(
	md5 PUBLIC
	.
	../../include
)

install(TARGETS md5 DESTINATION bin)
