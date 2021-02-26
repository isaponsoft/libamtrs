if(AMTRS_GUI_X11_ENABLE OR AMTRS_GUI_ALL_ENABLE)

find_package(X11 QUIET)
if(NOT X11_FOUND)
	include_directories(${X11_INCLUDE_DIRS})
	link_directories(${X11_LIBRARY_DIRS})
	target_link_libraries(amtrs ${X11_LIBRARIES})
endif()


find_package(XCB QUIET)
if(NOT XCB_FOUND)
	include_directories(${XCB_INCLUDE_DIRS})
	link_directories(${XCB_LIBRARY_DIRS})
	target_link_libraries(amtrs ${XCB_LIBRARIES})
endif()

endif()
