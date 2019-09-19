#!/bin/sh
# example)
#	AMTRS_ANDROID_ACTIVITY=org.libamtrs/android.app.NativeActivity
#	AMTRS_ANDROID_LOG=lamtrs
#	AMTRS_ANDROID_LOGFILE=android.log


exec_help() {
	echo "Google Android"
	echo ""
	echo "command"
	echo " help   This help."
	echo " build  Run gradle."
	echo " run    Run adb shell."
	echo " trace  Run ndk-stack. Need logcat stack dump."
	echo ""
	echo "example)"
	echo "$0 build -opt installDebug \; run"
	echo "$0 build -opt assembleRelease \; run"
}

exec_build() {
	if [ "${CONFIG}" = "debug" ]
	then
		(cd prj-android && ${CMD_PREFIX} gradlew${BAT_EXTRA} assembleDebug)
	else
		(cd prj-android && ${CMD_PREFIX} gradlew${BAT_EXTRA} assembleRelease)
	fi
	if [ "${AMTRS_ANDROID_OUT_DIR}" != "" ]
	then
		cp -r prj-android/app/build/outputs/apk/${CONFIG} ${AMTRS_ANDROID_OUT_DIR}
	fi
}


exec_clean() {

	(cd prj-android && ${CMD_PREFIX} gradlew${BAT_EXTRA} clean)
}

exec_run() {
	if [ "${CONFIG}" = "debug" ]
	then
		(cd prj-android && ${CMD_PREFIX} gradlew${BAT_EXTRA} installDebug)
	else
		(cd prj-android && ${CMD_PREFIX} gradlew${BAT_EXTRA} installRelease)
	fi
	adb${EXE_EXTRA} shell am start -n ${AMTRS_ANDROID_ACTIVITY}
}

exec_log() {
	adb${EXE_EXTRA} logcat | tee ${AMTRS_ANDROID_LOGFILE} | grep ${AMTRS_ANDROID_LOG} --line-buffered | sed -E "s/^.*${AMTRS_ANDROID_LOG} *: //g"
}


exec_trace() {
	${CMD_PREFIX} ndk-stack${CMD_EXTRA} -sym prj-android/app/build/intermediates/cmake/`echo ${CONFIG} | tr '[a-z]' '[A-Z]'`/obj/armeabi-v7a -dump ${AMTRS_ANDROID_LOGFILE} | sed -E 's/^.*(\.\.\/)//g'
}


. ${AMTRS_PATH}/tools/build-command.sh
