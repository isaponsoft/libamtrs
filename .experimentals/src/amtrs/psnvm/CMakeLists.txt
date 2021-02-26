cmake_minimum_required(VERSION 3.10.0)
project(passion)
enable_testing()

set(libAmtrs_DIR	"../..")
find_package(libAmtrs REQUIRED)

set(libPassion_DIR	"../..")
find_package(libPassion REQUIRED)


add_executable(
	psnvm
	main.cpp
)
target_include_directories(psnvm PRIVATE ${libAmtrs_INCLUDES} ${libPassion_INCLUDES})
target_link_libraries(psnvm ${libAmtrs_LIBSS} ${libPassion_LIBS})




add_test(basic_test	${CMAKE_SOURCE_DIR}/tests/test-001.psn)
