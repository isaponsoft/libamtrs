find_package(PkgConfig)

if(NOT X11_XCB_FIND_COMPONENTS)
    set(X11_XCB_FIND_COMPONENTS X11_XCB)
endif()

include(FindPackageHandleStandardArgs)
set(X11_XCB_FOUND			true)
#set(X11_XCB_INCLUDE_DIRS	"")
#set(X11_XCB_LIBRARIES		"")

pkg_check_modules(PC_X11_XCB QUIET X11_XCB)

find_path(X11_XCB_INCLUDE_DIRS NAMES xcb/xcb.h HINTS "")
find_library(X11_XCB_LIB_X11_XCB NAMES libX11-xcb.a HINTS "")
find_library(X11_XCB_LIB_XCB NAMES libxcb.a HINTS "")
find_library(X11_XCB_LIB_XAU NAMES libXau.a HINTS "")
find_library(X11_XCB_LIB_XDMPCP NAMES libXdmcp.a HINTS "")
set(X11_XCB_LIBRARIES	 ${X11_XCB_LIB_X11_XCB} ${X11_XCB_LIB_XCB} ${X11_XCB_LIB_XAU} ${X11_XCB_LIB_XDMPCP})

mark_as_advanced(X11_XCB_INCLUDE_DIRS X11_XCB_LIBRARIES)
