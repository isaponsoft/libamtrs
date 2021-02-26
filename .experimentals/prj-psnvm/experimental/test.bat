@ECHO OFF
SET _bin="..\..\bin\psnvm-win-%PROCESSOR_ARCHITECTURE%.exe"
REM %_bin% --dump-class-hierarchy --build-only test.psc
%_bin% test.psc
@ECHO %ERRORLEVEL%
