#!/bin/bash
#./augSesc -w1 -cconfigs/workFile/genome__64_1_8_2_Perfect.conf_0 -dconfigs/workFile/genome__64_1_8_2_Perfect.conf_0.report ../libapp/hello.sesc -t2 -g256 -s16 -n16384
./augSesc -w1 -cconfigs/workFile/genome__64_1_8_2_Perfect.conf_0 -dconfigs/workFile/genome__64_1_8_2_Perfect.conf_0.report ~/csl/sesc-cvs/tests/mcf < ~/csl/sesc-cvs/tests/mcf.in -t2 -g256 -s16 -n16384 
