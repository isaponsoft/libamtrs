cmake_minimum_required(VERSION 3.0.2)
project(bin2h)

set(AMTRS_APPNAME "bin2h")

file(GLOB AMTRS_APPSRC
	main.cpp
)

set(libAmtrs_DIR ../..)
include(${libAmtrs_DIR}/template.app-console.cmake)
