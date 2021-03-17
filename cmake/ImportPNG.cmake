# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
set(LIB_VERSION		16)
amtrs_add_external_license(PNG	"libpng" http://www.libpng.org/pub/png/src/libpng-LICENSE.txt)

amtrs_make_libname(__libname1	libpng${LIB_VERSION} _static d)


amtrs_find_libraries(png.h ${__libname1})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()



amtrs_external_add(
	${NAME}
	LIBS			${__libname1}
	URL				http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.xz?download
)
amtrs_external_deps(${NAME} ZLIB)

