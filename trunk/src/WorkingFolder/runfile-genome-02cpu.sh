#!/bin/bash
HOSTNAME=$(hostname)

# already tested
./augSesc -w1 -cconfigs/workFile/genome-02cpu-00.conf -dconfigs/workFile/genome-02cpu-00.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-00.out.$HOSTNAME

