@ECHO OFF
SET __DIR=%~dp0
SET __OUTDIR=%1

IF "%__OUTDIR%" == "" (
	ECHO "usage) build_and_copy OUTPUT-DIR"
	ECHO "OUTPUT-DIR : "-" ... No copy(Build only)."
	EXIT /b
)

IF NOT EXIST "%__OUTDIR%" (
	ECHO not such directory "%__OUTDIR%".
	EXIT /b
)


cmake %__DIR%
msbuild apgen.vcxproj /p:Configuration=Release;Platform=x64
IF ERRORLEVEL 1 (
	EXIT /b
)

IF "%__OUTDIR%" == "-" (
	EXIT /b
)

ECHO Copy to "%__OUTDIR%\apgen.exe"
COPY Release\apgen.exe "%__OUTDIR%\apgen.exe"
