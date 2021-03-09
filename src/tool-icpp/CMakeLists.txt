cmake_minimum_required(VERSION 3.0.2)
project(icpp)

set(AMTRS_APPNAME "icpp")

file(GLOB AMTRS_APPSRC
	main.cpp
	arguments.cpp
	build.cpp
	setup.cpp
)

set(AMTRS_SSL_ENABLE 1)
set(AMTRS_ZLIB_ENABLE 1)
set(AMTRS_ARCHIVE_ENABLE 1)
set(AMTRS_CONSOLE_ENABLE 1)
set(libAmtrs_DIR ../..)
include(${libAmtrs_DIR}/template.app-console.cmake)

install(TARGETS icpp DESTINATION bin)
