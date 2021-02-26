#!/bin/sh

ct () {
	testname="$1"
	clang -o work/${testname} -Wall -O2 -I ../../include ${testname}.c ; ./work/${testname} ; echo "${testname} : $?"
}

cppt () {
	testname="$1"
	clang++ -std=c++17 -o work/${testname}-cpp -Wall -I ../../include ${testname}.cpp ; ./work/${testname}-cpp ; echo "${testname} : $?"
}

if [ ! -d "work" ]
then
	mkdir work
fi

cppt "decl_class"
