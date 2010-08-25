#!/bin/bash
HOSTNAME=$(hostname)

./augSesc -w1 -cconfigs/workFile/genome-02cpu-01.conf -dconfigs/workFile/genome-02cpu-01.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-01.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-02.conf -dconfigs/workFile/genome-02cpu-02.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-02.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-03.conf -dconfigs/workFile/genome-02cpu-03.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-03.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-04.conf -dconfigs/workFile/genome-02cpu-04.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-04.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-05.conf -dconfigs/workFile/genome-02cpu-05.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-05.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-06.conf -dconfigs/workFile/genome-02cpu-06.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-06.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-07.conf -dconfigs/workFile/genome-02cpu-07.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-07.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-08.conf -dconfigs/workFile/genome-02cpu-08.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-08.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-09.conf -dconfigs/workFile/genome-02cpu-09.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-09.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-10.conf -dconfigs/workFile/genome-02cpu-10.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-10.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-11.conf -dconfigs/workFile/genome-02cpu-11.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-11.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-12.conf -dconfigs/workFile/genome-02cpu-12.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-12.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-13.conf -dconfigs/workFile/genome-02cpu-13.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-13.out.$HOSTNAME
./augSesc -w1 -cconfigs/workFile/genome-02cpu-14.conf -dconfigs/workFile/genome-02cpu-14.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-14.out.$HOSTNAME
