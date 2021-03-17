set(SRCS
	main.cpp
)

add_executable(licensepk ${SRCS})

target_link_libraries(licensepk amtrs_static)
install(TARGETS licensepk DESTINATION bin)
