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
_out="../../bin/psnvm-$(uname -s)-$(uname -m)"
_flags="-std=c++17 -Wall -ffunction-sections -o ${_out} -I../../include -fexceptions ../src/main.cpp"


#_defines="-DPASSION_COMPILER_SHOW_ERRORPOINT -DDEBUG"
#_defines="-DPASSION_VMBYTECODE_TRACE -DDEBUG"
#_defines="-DPASSION_ASSEMBLER_TRACE -DDEBUG"
#_defines="-DPASSION_COMPILER_SHOW_ERRORPOINT -DPASSION_ASSEMBLER_TRACE -DPASSION_VMBYTECODE_TRACE -DDEBUG"
_defines="-DPASSION_COMPILER_SHOW_ERRORPOINT -DPASSION_ASSEMBLER_TRACE -DPASSION_COMPILER_TRACE -DPASSION_VMBYTECODE_TRACE -DDEBUG"


${_compiler} ${_defines} ${_flags}
