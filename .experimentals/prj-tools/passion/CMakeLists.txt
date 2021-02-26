cmake_minimum_required(VERSION 3.10.2)
project(passion)
set(CMAKE_C_STANDARD	11)
set(CMAKE_CXX_STANDARD	17)

file(GLOB SRCS *.cpp)

add_compile_options("$<$<CONFIG:DEBUG>:/DDEBUG=1>")
add_compile_options("$<$<CONFIG:DEBUG>:/D_DEBUG=1>")
add_compile_options("$<$<CONFIG:DEBUG>:/D_CONSOLE=1>")
add_executable(psnvm ${SRCS})


target_include_directories(
	psnvm PUBLIC
	../../include
)


install(TARGETS psnvm DESTINATION bin)
