#!/bin/sh
#
#	case 1) compile all and run
#		/bin/sh test.sh
#
#	case 2) compile once and run
#		/bin/sh test.sh test-name [test args]
#
#	case 3) once run only
#		/bin/sh test.sh -run test-name [test args]
#
#
CPPFLAGS="-O3 -Wall -std=c++2a"

if [ "$CC" = "" ]
then
	CC=`which clang++`
	if [ "$CC" = "" ]
	then
		CC=`which g++`
		if [ "$CC" = "" ]
		then
			"compiler nothing."
			exit
		fi
	fi
fi
if [ "$CC" = "emcc" ]
then
	EMSCRIPTEN=1
fi


__runonly=
if [ "$1" = "-run" ]
then
	__runonly=1
	shift
fi
__only=$1
__argn=$#
if [ "$__argn" -ne "0" ]
then
	shift
	__args=$*
	__argn=$#
fi

test_script()
{
	if [ "$__only" = "" ] || [ "$__only" = "$1" ]
	then
		__cd=$(pwd)
		__bin=./tmp/$1
		__out=tmp/$1
		__src=src/$1.cpp
		__optfile=src/$1.opt
		if [ "$EMSCRIPTEN" = "1" ]
		then
			__bin="node tmp/$1.js"
			__out=tmp/$1.html
			__optfile=src/$1.opt-emcc
		fi
		__opt=${CPPFLAGS}
		if [ -f "${__optfile}" ]
		then
			__opt=${__opt} $(cat ${__optfile})
		fi

		if [ "$__runonly" = "" ]
		then
			if [ "$__only" = "" ]
			then
				${CC} -o ${__out} -I../include ${__opt} ${__src} 1> /dev/null 2> /dev/null
				if [ "$?" -ne "0" ]
				then
					echo "$1  NG(compile error)"
					return
				fi
			else
				echo ${CC} -o ${__out} -I../include ${__opt} ${__src}
				${CC} -o ${__out} -I../include ${__opt} ${__src}
				if [ "$?" -ne "0" ]
				then
					return
				fi
			fi
		fi

		if [ "$__only" = "" ]
		then
			${__bin} ${__args} > /dev/null
			if [ "$?" -ne "0" ]
			then
				echo "$1  NG"
			else
				echo "$1  OK"
			fi
		else
			${__bin} ${__args}
		fi
		cd ${__cd}
	fi
}



for f in src/*.cpp
do
	__filename=$(basename  $f .cpp)
	test_script "$__filename" $__opt
done
