@ECHO OFF
SET CFLAGS=/MD /EHsc /Ox

SET __runonly=
IF "%1"=="-run" (
	SET __runonly=1
	SHIFT
)

SET __only=
IF NOT "%1"=="" (
	SET __only=%1
	SHIFT
) ELSE (
	SET __only=
)



setlocal enabledelayedexpansion
FOR %%f IN (src\*.cpp) do (
	SET __filename=%%~nf
	SET __brunch_flag=FALSE
	SET __srcfile=src\!__filename!.cpp
	SET __objfile=tmp\!__filename!.obj
	SET __exefile=tmp\!__filename!.exe
	SET __optfile=src\!__filename!.opt-vc
	SET __opt=
	IF EXIST "!__optfile!" (
		FOR /f %%l IN (!__optfile!) DO (
			SET __opt=!__opt! %%l
		)
	)

	IF "!__only!"=="" (
		SET __brunch_flag=TRUE
	)
	IF "!__only!"=="!__filename!" SET __brunch_flag=TRUE

	IF "!__brunch_flag!" == "TRUE" (
		CALL :test_script %1 %2 %3 %4 %5 %6 %7 %8 %9
	)
)
endlocal

GOTO END


:test_script
	IF "!__runonly!" == "" (
		IF "!__only!" == "" (
			CL.exe /std:c++17 /Fe"%__exefile%" /Fo"%__objfile%" /I"..\include" %CFLAGS% %__opt% %__srcfile% > nul 2> nul
			IF ERRORLEVEL 1 (
				ECHO !__filename!  NG compile error
				EXIT /B %ERRORLEVEL%
			)
		) ELSE (
			CL.exe /std:c++17 /Fe"%__exefile%" /Fo"%__objfile%" /I"..\include" %CFLAGS% %__opt% %__srcfile%
			IF ERRORLEVEL 1 (
				EXIT /B %ERRORLEVEL%
			)
		)
	)

	IF "!__only!" == "" (
		!__exefile! %1 %2 %3 %4 %5 %6 %7 %8 %9 > nul
		IF ERRORLEVEL 1 (
			ECHO !__filename!  NG compile error
		) ELSE (
			ECHO !__filename!  OK
		)
	) ELSE (
		!__exefile! %1 %2 %3 %4 %5 %6 %7 %8 %9
	)

	EXIT /B %ERRORLEVEL%


:END
