# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_add_external_license(VORBIS	"zlib" https://raw.githubusercontent.com/madler/zlib/master/README)

if(MSVC)
amtrs_make_libname(__libname1	zlib static d)
else()
amtrs_make_libname(__libname1	libz "" "")
endif()

amtrs_find_libraries(zlib.h ${__libname1} ${__libname2})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


amtrs_external_add(
	${NAME}
	LIBS			${__libname1}
	URL				https://github.com/madler/zlib/archive/master.zip
	CMAKE_ARGS		-DBUILD_SHARED_LIBS=0
)
