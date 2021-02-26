# libamtrs cmake project

## cmakeプロジェクトから libamtrs を使う

libAmtrs_DIR に libamtrs のルートディレクトリのパスをセットし、find_package を呼び出してください。あとは必要なターゲットに対して target_include_directory, target_link_libraries を呼び出してください。

```cmake
set(libAmtrs_DIR	../libs/libamtrs)
find_package(libAmtrs)

target_include_directories(yourapp PUBLIC ${libAmtrs_INCLUDES})
target_link_libraries(yourapp ${libAmtrs_LIBS})
```

## 拡張モジュールを有効にする

libamtrsには広く使用されているライブラリを使用するための設定をいくつか付属しています。下の表はライブラリのプロジェクト名とlibamtrsにおけるモジュールの名称です。

これらはデフォルトでは使用しませんが、CMakeLists.txtに AMTRS_xxxx_ENABLE を定義することで使用できるようになります。

|project|name|note|
|--|--|--|
|libogg|OGG|Audio library|
|OpenCV|OPENCV||
|vulkan|VULKAN|3D Graphics api|

### 使用例

例えば、liboggとOpenCVを使用したい場合は以下のようになります。

```cmake
set(AMTRS_OGG_ENABLE    1 CACHE INTERNAL "" FORCE)
set(AMTRS_OPENCV_ENABLE 1 CACHE INTERNAL "" FORCE)

set(libAmtrs_DIR	../libs/libamtrs)
find_package(libAmtrs)

target_include_directories(yourapp PUBLIC ${libAmtrs_INCLUDES})
target_link_libraries(yourapp ${libAmtrs_LIBS})
```
