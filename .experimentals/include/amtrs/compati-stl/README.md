# compati

compati is compatible major library.

If your target platform is has not STL library, can use this compatible library.

You need addition include path to compiler options.

Compatible library path is "libamtrs/include/compati/stl/CLASS-NAME".

Using)

Use filesystem compatible library.

```shell
${CC} -std=c++17 -I libamtrs/include -I libamtrs/include/compati/stl/filesystem source-code
```

