#!/bin/bash
./augSesc -w1 -cconfigs/workFile/newfile.conf -dconfigs/workFile/temp.report benchmarks/vacation -c2 -n2 -q90 -u98 -r16384 -t4096
#./augSesc -cconfigs/workFile/memtest-directory-02cpu.conf -dconfigs/workFile/memtest-directory-02cpu.report ../libapp/memtest.sesc
#-w1 -cconfigs/workFile/genome-snoopy.conf -dconfigs/workFile/genome-snoopy.report benchmarks/genome -t2 -g256 -s16 -n16384
#./augSesc -w1 -cconfigs/workFile/genome__64_1_8_2_Perfect.conf_0 -dconfigs/workFile/genome__64_1_8_2_Perfect.conf_0.report ../libapp/hello.sesc -t2 -g256 -s16 -n16384
#./augSesc -w1 -cconfigs/workFile/genome__64_1_8_2_Perfect.conf_0 -dconfigs/workFile/genome__64_1_8_2_Perfect.conf_0.report ~/csl/sesc-cvs/tests/mcf < ~/csl/sesc-cvs/tests/mcf.in -t2 -g256 -s16 -n16384 
