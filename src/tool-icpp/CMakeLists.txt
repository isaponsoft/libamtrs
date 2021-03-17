set(SRCS
	arguments.cpp
	build.cpp
	icpp.hpp
	icpp-wrap.hpp
	main.cpp
	setup.cpp
	setup-visualstudio.cpp
)

add_executable(icpp ${SRCS})
#set_property(TARGET icpp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(icpp amtrs_static)
install(TARGETS icpp DESTINATION bin)
