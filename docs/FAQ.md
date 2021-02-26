# FAQ

## Q. cmark CMake compatibility error. 

```shell
Compatibility with CMake < 2.8.12 will be removed from a future version of
Cmake.
```

### A. remove add_compiler_export_flags()

1. Change ```cmake_minimum_required(VERSION 2.8)``` to ```cmake_minimum_required(VERSION 3.0)```.
2. Remove ```"add_compiler_export_flags()"``` in ```"cmark/extensions/CMakeLists.txt"```.

