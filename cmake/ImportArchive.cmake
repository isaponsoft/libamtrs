# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_make_libname(__libname1	archive _static "")
amtrs_add_external_license(ARCHIVE	"LibArchive" https://raw.githubusercontent.com/libarchive/libarchive/master/COPYING)

amtrs_find_libraries(archive.h ${__libname1})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


amtrs_external_add(
	${NAME}
	LIBS		${__libname1}
	URL			https://github.com/libarchive/libarchive/archive/master.zip
	CMAKE_ARGS	-DENABLE_WERROR=0
				-ENABLE_TEST=0
)
amtrs_external_deps(${NAME} ZLIB SSL)
