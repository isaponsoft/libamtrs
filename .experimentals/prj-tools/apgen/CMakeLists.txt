cmake_minimum_required(VERSION 3.10.2)

# Windows runtime library
if(POLICY CMP0091)
	cmake_policy(SET CMP0091 NEW)
endif()


project(apgen)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


file(GLOB GENS */*.cpp)

add_executable(
	apgen
	main.cpp
	download.cpp
	nullmodule.cpp
	update.cpp
	zip.cpp
	${GENS}
)


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	# CMP0091
	set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../libressl/CMakeLists.txt)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../libressl libressl)
else()
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
endif()



find_package(ZLIB)
if (ZLIB_FOUND)
	include_directories(apgen ${ZLIB_DEPS})
	target_link_libraries(apgen)
else()
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../zlib libz)
	set(ZLIB_LIBRARIES zlibstatic)
endif()



target_include_directories(
	apgen PUBLIC
	.
	../../include
	../../../zlib
	${CMAKE_CURRENT_BINARY_DIR}/libz
)
target_include_directories(apgen PRIVATE ssl crypto)

target_link_libraries(apgen PRIVATE ssl crypto ${ZLIB_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(apgen PRIVATE Threads::Threads)


#install(TARGETS apgen DESTINATION bin)
