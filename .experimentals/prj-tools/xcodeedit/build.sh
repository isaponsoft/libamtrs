#!/bin/sh
THISDIR=$(cd $(dirname $0) && pwd)

clang++ -std=c++17 -O2 -o xcodeedit `ls ${THISDIR}/*.cpp` -I ${THISDIR}/../../include
