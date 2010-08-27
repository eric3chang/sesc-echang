#!/bin/bash
HOSTNAME=$(hostname)

./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-00.conf -dconfigs/workFile/fft-02cpu-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-00.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-01.conf -dconfigs/workFile/fft-02cpu-01.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-01.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-02.conf -dconfigs/workFile/fft-02cpu-02.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-02.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-03.conf -dconfigs/workFile/fft-02cpu-03.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-03.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-04.conf -dconfigs/workFile/fft-02cpu-04.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-04.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-05.conf -dconfigs/workFile/fft-02cpu-05.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-05.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-06.conf -dconfigs/workFile/fft-02cpu-06.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-06.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-07.conf -dconfigs/workFile/fft-02cpu-07.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-07.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-08.conf -dconfigs/workFile/fft-02cpu-08.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-08.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-09.conf -dconfigs/workFile/fft-02cpu-09.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-09.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-10.conf -dconfigs/workFile/fft-02cpu-10.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-10.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-11.conf -dconfigs/workFile/fft-02cpu-11.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-11.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-12.conf -dconfigs/workFile/fft-02cpu-12.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-12.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-13.conf -dconfigs/workFile/fft-02cpu-13.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-13.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-02cpu-14.conf -dconfigs/workFile/fft-02cpu-14.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-02cpu-14.out.$HOSTNAME
