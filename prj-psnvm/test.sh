#!/bin/sh
_bin="../bin/psnvm-$(uname -s)-$(uname -m)"
${_bin} test/test-all.psc | grep -E "(OK|NG)"
