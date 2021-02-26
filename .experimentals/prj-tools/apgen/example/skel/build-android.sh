#!/bin/sh
__THIS=$(cd $(dirname $0); pwd)

if [ "$1" = "" ]
then
	BUILDOPT=assembleDebug
else
	BUILDOPT=$1
fi



for i in ..\lib ..\..\lib ..\libs ..\..\libs
do
	if [ -f "${__THIS}/${i}/libamtrs" ]
	then
		source "${__THIS}/${i}/libamtrs/setenv.sh"
	fi
done


apgen "${__THIS}" -M "android-gradle"
if [ "$?" = "0" ]
then
	/bin/sh gradlew ${BUILDOPT}
fi
