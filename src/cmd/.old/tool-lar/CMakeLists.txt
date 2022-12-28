set(SRCS
	main.cpp
)

add_executable(lar ${SRCS})
target_link_libraries(lar ${AMTRS_LIBRARY})
install(TARGETS lar DESTINATION bin)
