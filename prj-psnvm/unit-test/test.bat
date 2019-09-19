@ECHO OFF


SET testname=namespaces
CALL :CT
CALL :CPPT
exit /b


:CT
CL.EXE /nologo /W4 /MT /Ox /Fe"work\%testname%.exe" /Fo"work\%testname%.obj" /I..\..\include %testname%.c
work\%testname%.exe
echo %testname% : %ERRORLEVEL%
exit /b

:CPPT
CL.EXE /nologo /std:c++latest /W4 /MT /Ox /Fe"work\%testname%-cpp.exe" /Fo"work\%testname%-cpp.obj" /I..\..\include %testname%.cpp
work\%testname%-cpp.exe
echo %testname% : %ERRORLEVEL%
exit /b
