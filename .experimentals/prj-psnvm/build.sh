#!/bin/sh
_compiler=`which clang++`
if [ "${_compiler}" = "" ]
then
	_compiler=`which g++`
	if [ "${_compiler}" = "" ]
	then
		echo "error : nothing clang++ or g++."
		exit
	fi
fi
_out="../bin/psnvm-$(uname -s)-$(uname -m)"
${_compiler} -std=c++17 -O3 -Wall -ffunction-sections -o ${_out} -I../include -fexceptions src/main.cpp
