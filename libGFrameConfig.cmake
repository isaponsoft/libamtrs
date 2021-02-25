# ===================================================================================
#  Passion VM Config
#
#	libGFrame_LIBS
#	libGFrame_INCLUDES
#
# ===================================================================================
message("libGFrame_FOUND")


if(NOT libGFrame_FOUND)
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"libamtrs c++ std" FORCE)


if(NOT libAmtrs_FOUND)
set(libAmtrs_DIR	"${libGFrame_DIR}")
find_package(libAmtrs REQUIRED)
endif()


file(GLOB __SRCS	"${libGFrame_DIR}/src/gframe/*.cpp")


set(libGFrame_LIB		libgframe)
set(libGFrame_LIBS		${libGFrame_LIB} ${libAmtrs_LIBS})
set(libGFrame_INCLUDES	${libGFrame_DIR}/include/gframe ${libAmtrs_INCLUDES})


add_library(
	${libGFrame_LIB} STATIC ${__SRCS}
)


target_link_libraries(${libGFrame_LIB} ${libAmtrs_LIBS})

target_include_directories(
	${libGFrame_LIB} PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${libGFrame_INCLUDES}
)


endif()
