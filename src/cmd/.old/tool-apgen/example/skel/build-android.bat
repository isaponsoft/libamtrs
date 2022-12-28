@ECHO OFF

IF "%1" == "" (
	SET BUILDOPT=assembleDebug
) ELSE (
	SET BUILDOPT=%1
)



FOR %%I IN (..\lib ..\..\lib ..\libs ..\..\libs) DO (
	IF EXIST %~dp0..\%%I\%libamtrs% (
		call %~dp0..\%%I\libamtrs\setenv.bat
	)
)


apgen.exe %~dp0 -M "android-gradle"
IF NOT ERRORLEVEL 1 (
	call gradlew.bat %BUILDOPT%
)
