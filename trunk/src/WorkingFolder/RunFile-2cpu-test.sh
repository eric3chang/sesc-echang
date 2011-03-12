#!/bin/bash
HOSTNAME=$(hostname)

# already tested
#./augSesc -w1 -cconfigs/workFile/genome__64_1_8_2_Perfect.conf_0 -dconfigs/workFile/genome__64_1_8_2_Perfect.conf_0.report benchmarks/genome -t2 -g256 -s16 -n16384
./augSesc -w1 -cconfigs/workFile/vacation-low-02cpu.conf -dconfigs/workFile/vacation-low-02cpu.report benchmarks/vacation -c2 -n2 -q90 -u98 -r16384 -t4096 &> console-outputs/vacation-low-02cpu.out
./augSesc -w1 -cconfigs/workFile/vacation-high-02cpu.conf -dconfigs/workFile/vacation-high-02cpu.report benchmarks/vacation -c2 -n4 -q60 -u90 -r16384 -t4096 &> console-outputs/vacation-high-02cpu.out
./augSesc -w1 -cconfigs/workFile/yada-633-02cpu.conf -dconfigs/workFile/yada-633-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/633.2 &> console-outputs/yada-633-02cpu.out
./augSesc -w1 -cconfigs/workFile/yada-dots-02cpu.conf -dconfigs/workFile/yada-dots-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/dots.2 &> console-outputs/yada-dots-02cpu.out
./augSesc -w1 -cconfigs/workFile/yada-ladder-02cpu.conf -dconfigs/workFile/yada-ladder-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/ladder.2 &> console-outputs/yada-ladder-02cpu.out
./augSesc -w1 -cconfigs/workFile/yada-spiral-02cpu.conf -dconfigs/workFile/yada-spiral-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/spiral.2 &> console-outputs/yada-spiral-02cpu.out
./augSesc -w1 -cconfigs/workFile/yada-ttimeu10000-02cpu.conf -dconfigs/workFile/yada-ttimeu10000-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/ttimeu10000.2 &> console-outputs/yada-ttimeu10000-02cpu.out
# don't run the following because it might take too much time, because ttimeu10000 already takes a lot of time
#./augSesc -w1 -cconfigs/workFile/yada-ttimeu100000-02cpu.conf -dconfigs/workFile/yada-ttimeu100000-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/ttimeu100000.2 &> console-outputs/yada-ttimeu100000-02cpu.out
#./augSesc -w1 -cconfigs/workFile/yada-ttimeu1000000-02cpu.conf -dconfigs/workFile/yada-ttimeu1000000-02cpu.report benchmarks/yada -t2 -a20 -i yada-inputs/ttimeu1000000.2 &> console-outputs/yada-ttimeu1000000-02cpu.out

