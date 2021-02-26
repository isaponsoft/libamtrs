# gframe

Game 開発を行うためのフレームワークです。 amtrs-gframe ライブラリとして提供されます。

## CmakeLists.txt

```cmake
set(LIBAMTRS_GFRAME_APPCONFDIR ${CMAKE_SOURCE_DIR} CACHE STRING "" FORCE)
add_subdirectory(${LIBAMTRS_PATH})
target_link_libraries(myapp amtrs amtrs-gframe)
```

