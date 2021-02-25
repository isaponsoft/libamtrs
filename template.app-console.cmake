if(CMAKE_SYSTEM_NAME MATCHES "Windows")
add_definitions(-DCONSOLE=1)
add_definitions(-D_CONSOLE=1)
endif()

find_package(libAmtrs)


# Default source codes.
if (NOT AMTRS_APPSRC)
	file(GLOB AMTRS_APPSRC  "*.cpp" "src/*.cpp" "src/*/*.cpp")
endif ()


add_executable(${AMTRS_APPNAME} ${AMTRS_APPSRC})
target_include_directories(
	${AMTRS_APPNAME} PUBLIC
	inc
	include
	src
	${libAmtrs_INCLUDES}
)

target_link_libraries(${AMTRS_APPNAME} ${libAmtrs_LIBS})

