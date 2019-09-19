@ECHO OFF
SET _bin="..\bin\psnvm-win-%PROCESSOR_ARCHITECTURE%.exe"
%_bin% test/test-all.psc | find "NG"
