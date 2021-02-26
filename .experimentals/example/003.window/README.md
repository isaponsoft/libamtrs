# Hello world

This is buildsamle.

## Build

### MSVC

```shell
mkdir tmp
cd tmp
cmake.exe ${THIS_DIR} -DCMAKE_BUILD_TYPE=Debug
msbuild.vcxproj
```

### BSD/Linux

```shell
mkdir tmp
cd tmp
cmake ${THIS_DIR} -DCMAKE_BUILD_TYPE=Debug
make
```
