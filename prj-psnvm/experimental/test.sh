#!/bin/sh
_bin="../../bin/psnvm-$(uname -s)-$(uname -m)"
#${_bin} --build-only test.psc ; echo $?
${_bin} test.psc ; echo $?
