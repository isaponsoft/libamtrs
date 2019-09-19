#!/bin/sh
# example)
#	AMTRS_WIN32_EXE=mygame.exe
#	AMTRS_MSBUILD="/mnt/c/Program Files (x86)/Microsoft Visual Studio/2017/Community/MSBuild/15.0/Bin/MSBuild.exe"

if [ "${CONFIG}" = "" ]
then
	CONFIG=Debug
fi

if [ "${PLATFORM}" = "" ]
then
	PLATFORM=x64
fi


if [ "${AMTRS_MSBUILD}" = "" ]
then
	AMTRS_MSBUILD="/mnt/c/Program Files (x86)/Microsoft Visual Studio/2017/Community/MSBuild/15.0/Bin/MSBuild.exe"
fi

exec_help() {
	echo "Microsoft Windows Desktop Application"
	echo ""
	echo "help"
	echo "build"
	echo "run"
	echo ""
	echo "example)"
	echo "$0 -config Debug -platform x64 build run -opt --bg=night"
}


exec_build() {

	cmds=`echo $CONFIG | tr -s ':'`
	echo $cmds

	(cd prj-win32 && "$AMTRS_MSBUILD" /nologo /verbosity:minimal /p:Configuration=$CONFIG,Platform=$PLATFORM $*)
}


exec_run() {
	if [ "$PLATFORM" = "x86" ]
	then
		(cd asset && ../prj-win32/$CONFIG/${AMTRS_WIN32_EXE} $*)
	elif [ "$PLATFORM" = "x64" ]
	then
		(cd asset && ../prj-win32/x64/$CONFIG/${AMTRS_WIN32_EXE} $*)
	fi
}

. ${AMTRS_PATH}/tools/build-command.sh
