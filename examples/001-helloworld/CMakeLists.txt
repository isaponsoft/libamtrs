cmake_minimum_required(VERSION 3.16)
project(helloworld)


if(MSVC)
set(CMAKE_CXX_STANDARD			20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
set(CMAKE_CXX_STANDARD			17)
endif()


set(Amtrs_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Amtrs)


add_executable(helloworld main.cpp)
target_include_directories(helloworld PUBLIC ${AMTRS_INCLUDE_DIRS})
target_link_libraries(helloworld ${AMTRS_LIBRARY})
