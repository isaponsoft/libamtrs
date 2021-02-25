find_package(libAmtrs)


# Default source codes.
if (NOT AMTRS_APPSRC)
	file(GLOB AMTRS_APPSRC  "*.cpp" "src/*.cpp" "src/*/*.cpp")
endif ()



if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_executable(
		${AMTRS_APPNAME} WIN32
		${AMTRS_APPSRC}
	)
elseif (ANDROID)
	add_library(
		${AMTRS_APPNAME} SHARED
		${AMTRS_APPSRC}
	)
else ()
	add_executable(
		${AMTRS_APPNAME}
		${AMTRS_APPSRC}
	)
endif ()



target_include_directories(
	${AMTRS_APPNAME} PUBLIC
	inc
	include
	src
	${libAmtrs_INCLUDES}
)

target_link_libraries(${AMTRS_APPNAME} ${libAmtrs_LIBS})

