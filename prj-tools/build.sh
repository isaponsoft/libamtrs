#!/bin/sh
clang++ -std=c++17 -I../include -o md5 src/md5.cpp
./md5 -sr HelloWorld md5 nofile build.sh
md5 -rs HelloWorld md5 nofile build.sh
