@ECHO OFF
SET OUT_DIR=..\bin
SET OBJS=
SET CPP_FLAGS=/nologo /std:c++17 /W3 /DWIN32 /D_WIN32 /EHsc /MT /Ox /Oi /GL  /D "_CONSOLE" /FD /I"..\include"
SET CPP_DEBUG_FLAG=-DPASSION_COMPILER_SHOW_ERRORPOINT -DPASSION_ASSEMBLER_TRACE -DPASSION_VMBYTECODE_TRACE -DDEBUG

cl %CPP_FLAGS% /Fe"%OUT_DIR%\psnvm-win-%PROCESSOR_ARCHITECTURE%.exe" src\main.cpp user32.lib /link /subsystem:console /OPT:REF
REM cl %CPP_FLAGS% %CPP_DEBUG_FLAG% /Fe"%OUT_DIR%\psnvm.exe" src\main.cpp user32.lib /link /subsystem:console /OPT:REF
