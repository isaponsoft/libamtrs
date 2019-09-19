# 特殊な引数
#	-config コンフィグ
#	CONFIG という変数に値が格納されます。
# --------------------------------------------------
IS_MS=`uname -a | grep Microsoft`
if [ "$IS_MS" = "" ]
then
	CMD_PREFIX=.
	EXE_EXTRA=
	BAT_EXTRA=
	CMD_EXTRA=
else
	CMD_PREFIX="cmd.exe /C"
	EXE_EXTRA=.exe
	BAT_EXTRA=.bat
	CMD_EXTRA=.cmd
fi

parse_args() {
	while [ "$1" != "" ]
	do
		__cmd=$1; shift
		if [ "$__cmd" = "-config" ]
		then
			CONFIG=$1; shift
		elif [ "$__cmd" = "-platform" ]
		then
			PLATFORM=$1; shift
		elif [ "$__cmd" = "build" ]
		then
			__BUILD_EXEC=1
			__BUILD_OPT=`get_opt $*`
			parse_args `next_opt $*`
			break
		elif [ "$__cmd" = "run" ]
		then
			__RUN_EXEC=1
			__RUN_OPT=`get_opt $*`
			parse_args `next_opt $*`
			break
		elif [ "$__cmd" = "log" ]
		then
			__LOG_EXEC=1
			__LOG_OPT=`get_opt $*`
			parse_args `next_opt $*`
			break
		elif [ "$__cmd" = "clean" ]
		then
			__CLEAN_EXEC=1
			__CLEAN_OPT=`get_opt $*`
			parse_args `next_opt $*`
			break
		elif [ "$__cmd" = "trace" ]
		then
			__TRACE_EXEC=1
			__TRACE_OPT=`get_opt $*`
			parse_args `next_opt $*`
			break
		elif [ "$__cmd" = "help" ]
		then
			exec_help
			exit 0
			break
		else
			echo "Unkown command \"$__cmd\""
			exit 1
		fi
	done
}

get_opt() {
	if [ "$1" = "-opt" ]
	then
		shift
		while [ "$1" != "" ]
		do
			__cmd=$1; shift
			if [ "$__cmd" = ";" ]
			then
				break
			fi
			echo -n "$__cmd "
		done
	fi
}

next_opt() {
	if [ "$1" = "-opt" ]
	then
		while [ "$1" != "" ]
		do
			__cmd=$1; shift
			if [ "$__cmd" = ";" ]
			then
				break
			fi
		done
	fi
	echo $*
}

parse_args $*
if [ "$__CLEAN_EXEC" != "" ]
then
	exec_clean $__CLEAN_OPT
fi

if [ "$__BUILD_EXEC" != "" ]
then
	exec_build $__BUILD_OPT
	if [ "$?" != "0" ]
	then
		exit 1
	fi
fi
if [ "$__RUN_EXEC" != "" ]
then
	exec_run $__RUN_OPT
fi

if [ "$__TRACE_EXEC" != "" ]
then
	exec_trace $__TRACE_OPT
fi

if [ "$__LOG_EXEC" != "" ]
then
	exec_log $__LOG_OPT
fi
