# gframe

Game 開発を行うためのフレームワークです。

## CmakeLists.txt

```cmake
set(LIBAMTRS_GFRAME_APPCONFDIR ${CMAKE_SOURCE_DIR} CACHE STRING "" FORCE)
add_subdirectory(libamtrs)
target_link_libraries(myapp amtrs amtrs-gframe)
```

