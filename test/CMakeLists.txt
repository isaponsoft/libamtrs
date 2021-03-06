cmake_minimum_required(VERSION 3.0.2)
project(test)

set(AMTRS_APPNAME "test")

file(GLOB AMTRS_APPSRC
	main.cpp
	tests-filesystem-zip.cpp
	tests-io-crypto.cpp
	tests-io-read.cpp
	tests-io-streamif.cpp
	tests-string-wildcard.cpp
)

set(libAmtrs_DIR ..)
include(${libAmtrs_DIR}/template.app-console.cmake)
