cmake_minimum_required(VERSION 3.10.2)


project(inarikvs)
set(CMAKE_C_STANDARD		11)
set(CMAKE_CXX_STANDARD		17)


file(GLOB GENS *.cpp */*.cpp)

add_executable(
	inarikvs
	${GENS}
)


target_include_directories(
	inarikvs PUBLIC
	.
	../../include
)

add_subdirectory(../.. libamtrs)


find_package(Threads REQUIRED)
target_link_libraries(inarikvs PRIVATE Threads::Threads amtrs)
install(TARGETS inarikvs DESTINATION bin)
