# ===================================================================================
#  Passion VM Config
#
#	libPassion_LIBS
#	libPassion_INCLUDES
#
# ===================================================================================
if(NOT libPassion_FOUND)
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"libamtrs c++ std" FORCE)


if(NOT libAmtrs_FOUND)
set(libAmtrs_FOUND	"${libPassion_DIR}")
find_package(libAmtrs REQUIRED)
endif()


file(GLOB __PASSIONVM_SRCS	"${libPassion_DIR}/src/libpassion/*.cpp")

add_library(
	libpassion STATIC ${__PASSIONVM_SRCS} ${libAmtrs_LIBS}
)


set(libPassion_LIBS		libpassion)
set(libPassion_INCLUDES	${libPassion_DIR}/include)


target_include_directories(libpassion PRIVATE ${libPassion_INCLUDES})
endif()
