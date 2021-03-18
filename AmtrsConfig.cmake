function (amtrs_make_libname DESTNAME NAME STATIC_SUFFIX DEBUG_SUFFIX)
	if(WIN32)
		set(__tmp	${NAME})
	#	if (CMAKE_LINK_DEPENDS_NO_SHARED)
			set(__tmp	${__tmp}${STATIC_SUFFIX})
	#	endif()
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
amtrs_make_libname(_AMTRS_LIBNAME_R amtrs _static "")
amtrs_make_libname(_AMTRS_LIBNAME_D amtrs _static d)


find_path(AMTRS_INCLUDE_DIR NAMES amtrs/amtrs.hpp PATH_SUFFIXES include)

mark_as_advanced(AMTRS_INCLUDE_DIR)

find_library(AMTRS_LIBRARY_RELEASE	NAMES ${_AMTRS_LIBNAME_R} PATH_SUFFIXES lib)
find_library(AMTRS_LIBRARY_DEBUG	NAMES ${_AMTRS_LIBNAME_D} PATH_SUFFIXES lib)
include(SelectLibraryConfigurations)
select_library_configurations(AMTRS)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Amtrs REQUIRED_VARS AMTRS_LIBRARY AMTRS_INCLUDE_DIR)


if(AMTRS_FOUND)
    set(AMTRS_INCLUDE_DIRS ${AMTRS_INCLUDE_DIR})

    if(NOT AMTRS_LIBRARIES)
		set(AMTRS_LIBRARIES ${AMTRS_LIBRARY})
    endif()

    if(NOT TARGET AMTRS::AMTRS)
		add_library(AMTRS::AMTRS UNKNOWN IMPORTED)
		set_target_properties(AMTRS::AMTRS PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${AMTRS_INCLUDE_DIRS}")
		if(AMTRS_LIBRARY_RELEASE)
			set_property(TARGET AMTRS::AMTRS APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
			set_target_properties(AMTRS::AMTRS PROPERTIES IMPORTED_LOCATION_RELEASE "${AMTRS_LIBRARY_RELEASE}")
		endif()
		
		if(AMTRS_LIBRARY_DEBUG)
			set_property(TARGET AMTRS::AMTRS APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
			set_target_properties(AMTRS::AMTRS PROPERTIES IMPORTED_LOCATION_DEBUG "${AMTRS_LIBRARY_DEBUG}")
		endif()
		
		if(NOT AMTRS_LIBRARY_RELEASE AND NOT AMTRS_LIBRARY_DEBUG)
			set_property(TARGET AMTRS::AMTRS APPEND PROPERTY IMPORTED_LOCATION "${AMTRS_LIBRARY}")
		endif()
    endif()
endif()