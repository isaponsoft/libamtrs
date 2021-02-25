# ===================================================================================
#  Passion VM Config
#
#	libTimerCore_LIBS
#	libTimerCore_INCLUDES
#
# ===================================================================================
message("libTimerCore_FOUND")


if(NOT libTimerCore_FOUND)
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD	17	CACHE INTERNAL	"libamtrs c++ std" FORCE)


if(NOT libGFrame_FOUND)
set(libGFrame_DIR	"${libTimerCore_DIR}")
find_package(libGFrame REQUIRED)
endif()


file(GLOB __SRCS	"${libTimerCore_DIR}/src/timerapp-core/*.cpp" "${libTimerCore_DIR}/src/timerapp-core/*/*.cpp")


set(libTimerCore_LIB		libtimercore)
set(libTimerCore_LIBS		${libTimerCore_LIB} ${libGFrame_LIBS})
set(libTimerCore_INCLUDES	${libTimerCore_DIR}/utils/timerapp-core/inc ${libGFrame_INCLUDES})


add_library(
	${libTimerCore_LIB} STATIC ${__SRCS}
)


target_link_libraries(${libTimerCore_LIB} ${libGFrame})

target_include_directories(
	${libTimerCore_LIB} PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${libTimerCore_INCLUDES}
)
endif()
