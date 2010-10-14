#!/bin/bash
HOSTNAME=$(hostname)

./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-00.conf -dconfigs/workFile/fft-016-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-00.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-01.conf -dconfigs/workFile/fft-016-0001-0002-01.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-01.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-02.conf -dconfigs/workFile/fft-016-0001-0002-02.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-02.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-03.conf -dconfigs/workFile/fft-016-0001-0002-03.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-03.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-04.conf -dconfigs/workFile/fft-016-0001-0002-04.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-04.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-05.conf -dconfigs/workFile/fft-016-0001-0002-05.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-05.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-06.conf -dconfigs/workFile/fft-016-0001-0002-06.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-06.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-07.conf -dconfigs/workFile/fft-016-0001-0002-07.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-07.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-08.conf -dconfigs/workFile/fft-016-0001-0002-08.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-08.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-09.conf -dconfigs/workFile/fft-016-0001-0002-09.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-09.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-10.conf -dconfigs/workFile/fft-016-0001-0002-10.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-10.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-11.conf -dconfigs/workFile/fft-016-0001-0002-11.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-11.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-12.conf -dconfigs/workFile/fft-016-0001-0002-12.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-12.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-13.conf -dconfigs/workFile/fft-016-0001-0002-13.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-13.out.$HOSTNAME
./augSesc-release  -cconfigs/workFile/fft-016-0001-0002-14.conf -dconfigs/workFile/fft-016-0001-0002-14.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-016-0001-0002-14.out.$HOSTNAME
