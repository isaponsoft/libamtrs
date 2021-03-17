# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_make_libname(__libname1	vorbis "" "")
amtrs_make_libname(__libname2	vorbisfile "" "")
amtrs_add_external_license(VORBIS	"vorbis" https://raw.githubusercontent.com/xiph/vorbis/master/COPYING)


amtrs_find_libraries(vorbis/vorbisfile.h ${__libname1} ${__libname2})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


amtrs_external_add(
	${NAME}
	LIBS			${__libname1} ${__libname2}
	URL 			https://downloads.xiph.org/releases/vorbis/libvorbis-1.3.7.zip
)
amtrs_external_deps(${NAME} OGG)
