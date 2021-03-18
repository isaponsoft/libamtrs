# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
#find_package(OpenSSL QUIET)
#if(OpenSSL_FOUND)
#	set(MODULE_INCLUDE_DIRS	${OPENSSL_INCLUDE_DIR})
#	set(MODULE_LIBRARIES	${OPENSSL_LIBRARIES})
#	return()
#endif()
amtrs_add_external_license(SSL	"LibreSSL" https://raw.githubusercontent.com/libressl/libressl/master/src/LICENSE)



set(__crypto_suffix	"")
set(__ssl_suffix	"")
set(__tls_suffix	"")
if(WIN32)
set(__crypto_suffix	-46)
set(__ssl_suffix	-48)
set(__tls_suffix	-20)
endif()
amtrs_make_libname(__libname1	crypto${__crypto_suffix} "" "")
amtrs_make_libname(__libname2	ssl${__ssl_suffix} "" "")
amtrs_make_libname(__libname3	tls${__tls_suffix} "" "")


amtrs_find_libraries(tls.h ${__libname3} ${__libname2} ${__libname1})
if(_INCLUDE_DIR)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


set(__SSL_extra_cmakeargs)
if(MSVC)
	if(NOT "${CMAKE_MSVC_RUNTIME_LIBRARY}" MATCHES "DLL")
	list(APPEND __SSL_extra_cmakeargs	"-DUSE_STATIC_MSVC_RUNTIMES=ON")
	endif()
endif()


amtrs_external_add(
	SSL
	LIBS			${__libname1} ${__libname2} ${__libname3}
	URL				https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.3.0.tar.gz
	CMAKE_ARGS		${__SSL_extra_cmakeargs}
)
