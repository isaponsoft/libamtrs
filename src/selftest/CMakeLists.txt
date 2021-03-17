file(GLOB SRCS
	main.cpp
	tests-filesystem-zip.cpp
	tests-io-crypto.cpp
	tests-io-read.cpp
	tests-io-streamif.cpp
	tests-string-wildcard.cpp
)


add_executable(selftest ${SRCS})
target_link_libraries(selftest amtrs_static)
