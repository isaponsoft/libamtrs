# Copyright (c) 2019, isaponsoft (Isao Shibuya) All rights reserved.
# Use of this source code is governed by a BSD-style  license that
# can be found in the LICENSE file.



# =========================================================
# モジュールのインポート。モジュール内のワーク変数のスコープを閉じ込める
# ---------------------------------------------------------
#	include(cmake/Import${NAME})
#
function(amtrs_import __NAME)
	string(TOUPPER ${__NAME} UPPER_NAME)
	option(AMTRS_${UPPER_NAME}_ENABLE "Amtrs enable ${UPPER_NAME} module." ON)
	if(${AMTRS_ALL_ENABLE} OR ${AMTRS_${UPPER_NAME}_ENABLE})
		set(NAME	${UPPER_NAME})

		set(MODULE_INCLUDE_DIRS)
		set(MODULE_LIBRARIES)
		#if(NOT ${NAME}_LIBRARIES AND NOT ${NAME}_INCLUDE_DIRS)
			include(Import${__NAME})
			set(${NAME}_INCLUDE_DIRS	${MODULE_INCLUDE_DIRS})
			set(${NAME}_LIBRARIES		${MODULE_LIBRARIES})
		#endif()
		list(APPEND AMTRS_DEPENENCY_INCLUDE_DIRS	${${NAME}_INCLUDE_DIRS})
		list(APPEND AMTRS_LINK_LIBRARIES			${${NAME}_LIBRARIES})

		# いくつかの変数はスコープの外へ取り出す
		set(AMTRS_DEFINES					"AMTRS_${UPPER_NAME}_ENABLE=1" ${AMTRS_DEFINES}	PARENT_SCOPE)
		set(AMTRS_DEPENENCY_INCLUDE_DIRS	${AMTRS_DEPENENCY_INCLUDE_DIRS}					PARENT_SCOPE)
		set(AMTRS_LINK_LIBRARIES			${AMTRS_LINK_LIBRARIES}							PARENT_SCOPE)
		set(AMTRS_${UPPER_NAME}_EXTERNAL	${AMTRS_${UPPER_NAME}_EXTERNAL}					PARENT_SCOPE)

		set(${NAME}_INCLUDE_DIR		${NAME}_INCLUDE_DIR		PARENT_SCOPE)
		set(${NAME}_LIBRARY			${NAME}_LIBRARY			PARENT_SCOPE)
		set(${NAME}_INCLUDE_DIRS	${NAME}_INCLUDE_DIRS	PARENT_SCOPE)
		set(${NAME}_LIBRARIES		${NAME}_LIBRARIES		PARENT_SCOPE)

	endif()
endfunction()



# =========================================================
# ライブラリ名に静的ライブラリ、デバッグライブラリのサフィックスを追加する
# ---------------------------------------------------------
# amtrs_make_libname(PNG_LIBRARY libpng _static d)
# PNG_LIBRARY = libpng or libpngd or libpng_static or libpng_staticd
function (amtrs_make_libname DESTNAME NAME STATIC_SUFFIX DEBUG_SUFFIX)
	if(WIN32)
		set(__tmp	${NAME})
		if (CMAKE_LINK_DEPENDS_NO_SHARED)
			set(__tmp	${__tmp}${STATIC_SUFFIX})
		endif()
		if(CMAKE_BUILD_TYPE MATCHES Debug)
			set(__tmp	${__tmp}${DEBUG_SUFFIX})
		endif()
	else()
		set(__tmp	${NAME})
		string(REGEX REPLACE "^lib" "" __tmp "${__tmp}")
		set(__tmp	lib${__tmp})
	endif()
	set(${DESTNAME}	${__tmp} PARENT_SCOPE)
endfunction()




# =========================================================
# External_Add を呼び出す際に MSVC_RUNTIME の情報を引き継ぐ
# ---------------------------------------------------------
#	amtrs_external_add(png URL http://foo/bar CMAKE_ARGS -DCMAKE_xxx)
#	URL以外は指定しないこと！
#	* gitもsvnもmercurialもインストールされていない環境がある
#
function(amtrs_external_add NAME)
	set("AMTRS_${NAME}_EXTERNAL" ON PARENT_SCOPE)
	set(_CMAKE_ARGS)

	list(APPEND _CMAKE_ARGS	-DCMAKE_INSTALL_PREFIX=${AMTRS_DEPENENCY_INSTALL_DIR})
	list(APPEND _CMAKE_ARGS	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
	list(APPEND _CMAKE_ARGS	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
	list(APPEND _CMAKE_ARGS	-DCMAKE_LINK_DEPENDS_NO_SHARED=${CMAKE_LINK_DEPENDS_NO_SHARED})

	if(MSVC)
		# cmake after 3.16
		list(APPEND _CMAKE_ARGS		"-DMSVC=ON")
		list(APPEND _CMAKE_ARGS		"-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")

		# cmake under 3.16
		if(NOT "${CMAKE_MSVC_RUNTIME_LIBRARY}" MATCHES "DLL")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_C_FLAGS_DEBUG=/MTd")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_CXX_FLAGS_DEBUG=/MTd")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_C_FLAGS_RELEASE=/MT")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_CXX_FLAGS_RELEASE=/MT")
		else()
		list(APPEND _CMAKE_ARGS	"-DCMAKE_C_FLAGS_DEBUG=/MDd")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_CXX_FLAGS_DEBUG=/MDd")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_C_FLAGS_RELEASE=/MD")
		list(APPEND _CMAKE_ARGS	"-DCMAKE_CXX_FLAGS_RELEASE=/MD")
		endif()
	endif()

	set(__amtrs__external_add_OPTIONS)
	set(__amtrs__external_add_MODE	0)
	set(__amtrs__external_add_LIBS)

	math(EXPR lastIndex "${ARGC}-1")
    foreach(index RANGE 1 ${lastIndex})
		if("${ARGV${index}}" STREQUAL "LIBS")
			set(__amtrs__external_add_MODE	1)
			continue()
		elseif("${ARGV${index}}" STREQUAL "URL")
			set(__amtrs__external_add_MODE	2)
			set(__amtrs__external_add_OPTIONS ${ARGV${index}})
			continue()
		elseif("${ARGV${index}}" STREQUAL "CMAKE_ARGS")
			set(__amtrs__external_add_MODE	3)
			continue()
		endif()
		if("${__amtrs__external_add_MODE}" EQUAL "1")
			set(__tmp	"${AMTRS_DEPENENCY_INSTALL_DIR}/lib/${ARGV${index}}${CMAKE_STATIC_LIBRARY_SUFFIX}")
			if ("${__amtrs__external_add_LIBS}")
				set(APPEND __amtrs__external_add_LIBS ${__tmp})
			else()
				list(APPEND __amtrs__external_add_LIBS ${__tmp})
			endif()
		elseif("${__amtrs__external_add_MODE}" EQUAL "3")
			list(APPEND _CMAKE_ARGS ${ARGV${index}})
		else()
			list(APPEND __amtrs__external_add_OPTIONS ${ARGV${index}})
		endif()
    endforeach()	

	list(REMOVE_DUPLICATES _CMAKE_ARGS)

	include(ExternalProject)
	ExternalProject_Add(
		${NAME}_ext
		${__amtrs__external_add_OPTIONS}
		CMAKE_ARGS		${_CMAKE_ARGS}
	)
	add_dependencies(${AMTRS_LIBRARY} ${NAME}_ext)

	#set(MODULE_LIBRARIES		"${AMTRS_DEPENENCY_INSTALL_DIR}/lib/${__libname1}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set(MODULE_LIBRARIES	"${__amtrs__external_add_LIBS}"	PARENT_SCOPE)

	unset(__amtrs__external_add_OPTIONS)
	unset(__amtrs__external_add_ARGSMODE)

endfunction()


# =========================================================
# External_Add の依存関係を追加するユーティリティ
# ---------------------------------------------------------
# amtrs_external_deps(PNG ZLIB)
#	if(AMTRS_ZLIB_EXTERNAL)
#		add_dependencies(PNG ZLIB)
#	endif()
function(amtrs_external_deps NAME)
	set(__tmp			FALSE)

	math(EXPR __tmp_lastindex "${ARGC}-1")
    foreach(__tmp_index RANGE 1 ${__tmp_lastindex})
		set(__DEPNAME	${ARGV${__tmp_index}})
		string(TOUPPER ${__DEPNAME} __TMPNAME)
		if(AMTRS_${__TMPNAME}_EXTERNAL)
			if(__tmp)
				list(APPEND __tmp ${__DEPNAME}_ext)
			else()
				set(__tmp ${__DEPNAME}_ext)
			endif()
		endif()
    endforeach()
	if(__tmp)
		add_dependencies(${NAME}_ext ${__tmp})
	endif()
endfunction()



# =========================================================
# ヘッダとライブラリを検索する
# ---------------------------------------------------------
# amtrs_find_libraries(HeaderFilename [LibraryFilename...]
#
# ひとつでも見つからない項目があれば全て NOTFOUND をセットする。
# RESULT
# 	_INCLUDE_DIR
#	_LIBRARIES
#
function(amtrs_find_libraries HEADER)
	# Clear
	set(_INCLUDE_DIR	NOTFOUND	PARENT_SCOPE)
	set(_LIBRARIES		NOTFOUND	PARENT_SCOPE)

	set(_PATHS			"${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${CMAKE_INSTALL_PREFIX}")

	set(__include		NOTFOUND)
	set(__libs			NOTFOUND)
	set(__tmp			NOTFOUND)
	find_path(__tmp NAMES ${HEADER} PATHS ${_PATHS} PATH_SUFFIXES include NO_DEFAULT_PATH)
	if(__tmp)
		set(__include ${__tmp})
	else()
		message("${HEADER} : ${HEADER} NOTFOUND")
		return()
	endif()


	math(EXPR __lastindex "${ARGC}-1")
    foreach(__index RANGE 1 ${__lastindex})
		set(__tmp		NOTFOUND)
		set(__filename	${ARGV${__index}})
		if(NOT MSVC)
			string(REGEX REPLACE "^lib" "" __filename "${__filename}")
		endif()
		find_library(__tmp NAMES ${__filename} PATHS ${_PATHS} PATH_SUFFIXES lib NO_DEFAULT_PATH)
		if(__tmp)
			if (__libs)
				list(APPEND __libs ${__tmp})
			else()
				set(__libs ${__tmp})
			endif()
		else()
			message("${HEADER} : ${__filename} NOTFOUND")
			return()
		endif()
    endforeach()
	message("${HEADER} : ${__include} ${__libs}")
	set(_INCLUDE_DIR	${__include}	PARENT_SCOPE)
	set(_LIBRARIES		${__libs}		PARENT_SCOPE)
endfunction()


macro(amtrs_add_external_license TAG NAME URL)
list(APPEND	AMTRS_ADD_LICENCES	${TAG})
set(AMTRS_${TAG}_LICENSE_URL	${URL})
set(AMTRS_ADD_LICENCES			${AMTRS_ADD_LICENCES}			PARENT_SCOPE)
set(AMTRS_${TAG}_NAME			${NAME}							PARENT_SCOPE)
set(AMTRS_${TAG}_LICENSE_URL	${AMTRS_${TAG}_LICENSE_URL}		PARENT_SCOPE)
endmacro()
