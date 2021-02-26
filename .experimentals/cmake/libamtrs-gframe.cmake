#if(NOT EXISTS ${LIBAMTRS_GFRAME_APPCONFDIR}/app-config.hpp)
#	message(FATAL_ERROR "${LIBAMTRS_GFRAME_APPCONFDIR}/app-config.hpp not found.")
#endif()
file(GLOB __AMTRS_GFRAME_SRCS src/gframe/*.cpp)
add_library(amtrs-gframe STATIC ${__AMTRS_GFRAME_SRCS})
include_directories(
	include
	${LIBAMTRS_GFRAME_APPCONFDIR}
)
target_link_libraries(amtrs-gframe amtrs)
