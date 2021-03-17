if(CMAKE_SYSTEM_NAME MATCHES "Windows")
add_definitions(-DCONSOLE=1)
add_definitions(-D_CONSOLE=1)
endif()

if(MSVC)
set(CMAKE_CXX_STANDARD	20	CACHE INTERNAL	"libamtrs c++ std" FORCE)
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
else()
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"libamtrs c++ std" FORCE)
endif()




find_path	(AMTRS_INCLUDE_DIR	NAMES amtrs/amtrs.hpp	PATH ${libAmtrs_DIR}	PATH_SUFFIXES 	include)
find_library(AMTRS_LIBRARY		NAMES amtrs_static		PATH ${libAmtrs_DIR}	PATH_SUFFIXES 	lib)
if (NOT AMTRS_INCLUDE_DIR OR NOT AMTRS_LIBRARY)
	add_subdirectory(${libAmtrs_DIR} EXCLUDE_FROM_ALL build-amtrs)
endif()



# Default source codes.
if (NOT AMTRS_APPSRC)
	file(GLOB AMTRS_APPSRC  "*.cpp" "src/*.cpp" "src/*/*.cpp")
endif ()

message("AMTRS_APPSRC : ${AMTRS_APPSRC}")

add_executable(${AMTRS_APPNAME} ${AMTRS_APPSRC})
target_include_directories(
	${AMTRS_APPNAME} PRIVATE
	inc
	include
	src
	${AMTRS_INCLUDE_DIR}
)
target_link_libraries(${AMTRS_APPNAME} ${AMTRS_LIBRARY})
