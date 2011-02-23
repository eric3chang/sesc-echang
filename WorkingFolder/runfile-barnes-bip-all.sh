#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-2-16-1024.conf -dconfigs/workFile/barnes-bip-2-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2 &> console-outputs/barnes-bip-2-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-2-32-1024.conf -dconfigs/workFile/barnes-bip-2-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2 &> console-outputs/barnes-bip-2-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-2-64-1024.conf -dconfigs/workFile/barnes-bip-2-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2 &> console-outputs/barnes-bip-2-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-2-128-1024.conf -dconfigs/workFile/barnes-bip-2-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2 &> console-outputs/barnes-bip-2-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-4-16-1024.conf -dconfigs/workFile/barnes-bip-4-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4 &> console-outputs/barnes-bip-4-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-4-32-1024.conf -dconfigs/workFile/barnes-bip-4-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4 &> console-outputs/barnes-bip-4-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-4-64-1024.conf -dconfigs/workFile/barnes-bip-4-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4 &> console-outputs/barnes-bip-4-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-4-128-1024.conf -dconfigs/workFile/barnes-bip-4-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4 &> console-outputs/barnes-bip-4-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-8-16-1024.conf -dconfigs/workFile/barnes-bip-8-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8 &> console-outputs/barnes-bip-8-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-8-32-1024.conf -dconfigs/workFile/barnes-bip-8-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8 &> console-outputs/barnes-bip-8-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-8-64-1024.conf -dconfigs/workFile/barnes-bip-8-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8 &> console-outputs/barnes-bip-8-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-8-128-1024.conf -dconfigs/workFile/barnes-bip-8-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8 &> console-outputs/barnes-bip-8-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-16-16-1024.conf -dconfigs/workFile/barnes-bip-16-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16 &> console-outputs/barnes-bip-16-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-16-32-1024.conf -dconfigs/workFile/barnes-bip-16-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16 &> console-outputs/barnes-bip-16-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-16-64-1024.conf -dconfigs/workFile/barnes-bip-16-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16 &> console-outputs/barnes-bip-16-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-16-128-1024.conf -dconfigs/workFile/barnes-bip-16-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16 &> console-outputs/barnes-bip-16-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-32-16-1024.conf -dconfigs/workFile/barnes-bip-32-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32 &> console-outputs/barnes-bip-32-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-32-32-1024.conf -dconfigs/workFile/barnes-bip-32-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32 &> console-outputs/barnes-bip-32-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-32-64-1024.conf -dconfigs/workFile/barnes-bip-32-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32 &> console-outputs/barnes-bip-32-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/barnes-bip-32-128-1024.conf -dconfigs/workFile/barnes-bip-32-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32 &> console-outputs/barnes-bip-32-128-1024.out.$HOSTNAME
