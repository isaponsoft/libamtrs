## Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.
amtrs_make_libname(__libname1	ogg "" "")
amtrs_add_external_license(OGG	"ogg" https://raw.githubusercontent.com/xiph/ogg/master/COPYING)


amtrs_find_libraries(ogg/ogg.h ${__libname1})
if(_LIBRARIES)
	set(MODULE_INCLUDE_DIRS	${_INCLUDE_DIR})
	set(MODULE_LIBRARIES	${_LIBRARIES})
	return()
endif()


amtrs_external_add(
	${NAME}
	LIBS			${__libname1}
	URL 			https://downloads.xiph.org/releases/ogg/libogg-1.3.4.zip
)
