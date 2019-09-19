#!/bin/sh
#
# Require)
# 	AMTRS_IOS_WORKSPACE=mygame.xcworkspace
# 	AMTRS_IOS_SCHEME=mygamescheme
#
# Optional)
# 	AMTRS_IOS_ARCHIVE_DIR=out/${AMTRS_IOS_SCHEME}.xcarchive
# 	AMTRS_IOS_ADHOC_DIR=out/${AMTRS_IOS_SCHEME}.adhoc
#

exec_help() {
	echo "Apple iOS Build & Archive"
	echo "sh ios -config Debug build"
	echo "ios unsupported 'run' command."
	echo "Please first security unlock-keychain"
}


exec_clean() {

	if [ "${AMTRS_IOS_ARCHIVE_DIR}" = "" ]
	then
		AMTRS_IOS_ARCHIVE_DIR=./out/${AMTRS_IOS_SCHEME}.xcarchive
	fi
	if [ "${AMTRS_IOS_ADHOC_DIR}" = "" ]
	then
		AMTRS_IOS_ADHOC_DIR=./out/${AMTRS_IOS_SCHEME}.adhoc
	fi

	(cd prj-ios && xcodebuild \
		-workspace ${AMTRS_IOS_WORKSPACE} \
		-scheme ${AMTRS_IOS_SCHEME} \
		-archivePath ${AMTRS_IOS_ARCHIVE_DIR}\
		-configuration $CONFIG clean)
}


exec_build_only() {

	if [ "${AMTRS_IOS_ARCHIVE_DIR}" = "" ]
	then
		AMTRS_IOS_ARCHIVE_DIR=./out/${AMTRS_IOS_SCHEME}.xcarchive
	fi
	if [ "${AMTRS_IOS_ADHOC_DIR}" = "" ]
	then
		AMTRS_IOS_ADHOC_DIR=./out/${AMTRS_IOS_SCHEME}.adhoc
	fi

	echo xcodebuild -workspace ${AMTRS_IOS_WORKSPACE} -scheme ${AMTRS_IOS_SCHEME} -archivePath ${AMTRS_IOS_ARCHIVE_DIR} -configuration $CONFIG archive $*

	(cd prj-ios && xcodebuild \
		-workspace ${AMTRS_IOS_WORKSPACE} \
		-scheme ${AMTRS_IOS_SCHEME} \
		-archivePath ${AMTRS_IOS_ARCHIVE_DIR}\
		-configuration $CONFIG build $*)
}


exec_build() {

	if [ "${AMTRS_IOS_ARCHIVE_DIR}" = "" ]
	then
		AMTRS_IOS_ARCHIVE_DIR=./out/${AMTRS_IOS_SCHEME}.xcarchive
	fi
	if [ "${AMTRS_IOS_ADHOC_DIR}" = "" ]
	then
		AMTRS_IOS_ADHOC_DIR=./out/${AMTRS_IOS_SCHEME}.adhoc
	fi

	echo xcodebuild -workspace ${AMTRS_IOS_WORKSPACE} -scheme ${AMTRS_IOS_SCHEME} -archivePath ${AMTRS_IOS_ARCHIVE_DIR} -configuration $CONFIG archive $*

	(cd prj-ios && xcodebuild \
		-workspace ${AMTRS_IOS_WORKSPACE} \
		-scheme ${AMTRS_IOS_SCHEME} \
		-archivePath ${AMTRS_IOS_ARCHIVE_DIR}\
		-configuration $CONFIG archive $* \
	&& xcodebuild \
		-exportArchive "-allowProvisioningUpdates" \
		-archivePath ${AMTRS_IOS_ARCHIVE_DIR}\
		-exportPath ${AMTRS_IOS_ADHOC_DIR} \
		-exportOptionsPlist ExportAdHoc.plist \
		$*)
}

exec_run() {

	if [ "${AMTRS_IOS_ADHOC_DIR}" = "" ]
	then
		AMTRS_IOS_ADHOC_DIR=./out/${AMTRS_IOS_SCHEME}.adhoc
	fi

	(cd prj-ios && cd ${AMTRS_IOS_ADHOC_DIR} && ios-deploy --bundle ios.ipa)
}

exec_log() {
	echo log moniter start.
	idevicesyslog | sed -e 's/^.*<[A-Za-z]*>: //' 
	
}

. ${AMTRS_PATH}/tools/build-command.sh
