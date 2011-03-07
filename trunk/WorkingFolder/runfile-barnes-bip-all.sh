#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-1-1024.conf -dconfigs/workFile/barnes-bip-2-1-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-2-1024.conf -dconfigs/workFile/barnes-bip-2-2-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-4-1024.conf -dconfigs/workFile/barnes-bip-2-4-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-8-1024.conf -dconfigs/workFile/barnes-bip-2-8-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-16-1024.conf -dconfigs/workFile/barnes-bip-2-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-32-1024.conf -dconfigs/workFile/barnes-bip-2-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-64-1024.conf -dconfigs/workFile/barnes-bip-2-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-2-128-1024.conf -dconfigs/workFile/barnes-bip-2-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu2-special &> console-outputs/barnes-bip-2-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-1-1024.conf -dconfigs/workFile/barnes-bip-4-1-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-2-1024.conf -dconfigs/workFile/barnes-bip-4-2-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-4-1024.conf -dconfigs/workFile/barnes-bip-4-4-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-8-1024.conf -dconfigs/workFile/barnes-bip-4-8-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-16-1024.conf -dconfigs/workFile/barnes-bip-4-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-32-1024.conf -dconfigs/workFile/barnes-bip-4-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-64-1024.conf -dconfigs/workFile/barnes-bip-4-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-4-128-1024.conf -dconfigs/workFile/barnes-bip-4-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu4-special &> console-outputs/barnes-bip-4-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-1-1024.conf -dconfigs/workFile/barnes-bip-8-1-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-2-1024.conf -dconfigs/workFile/barnes-bip-8-2-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-4-1024.conf -dconfigs/workFile/barnes-bip-8-4-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-8-1024.conf -dconfigs/workFile/barnes-bip-8-8-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-16-1024.conf -dconfigs/workFile/barnes-bip-8-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-32-1024.conf -dconfigs/workFile/barnes-bip-8-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-64-1024.conf -dconfigs/workFile/barnes-bip-8-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-8-128-1024.conf -dconfigs/workFile/barnes-bip-8-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu8-special &> console-outputs/barnes-bip-8-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-1-1024.conf -dconfigs/workFile/barnes-bip-16-1-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-2-1024.conf -dconfigs/workFile/barnes-bip-16-2-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-4-1024.conf -dconfigs/workFile/barnes-bip-16-4-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-8-1024.conf -dconfigs/workFile/barnes-bip-16-8-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-16-1024.conf -dconfigs/workFile/barnes-bip-16-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-32-1024.conf -dconfigs/workFile/barnes-bip-16-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-64-1024.conf -dconfigs/workFile/barnes-bip-16-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-16-128-1024.conf -dconfigs/workFile/barnes-bip-16-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu16-special &> console-outputs/barnes-bip-16-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-1-1024.conf -dconfigs/workFile/barnes-bip-32-1-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-2-1024.conf -dconfigs/workFile/barnes-bip-32-2-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-4-1024.conf -dconfigs/workFile/barnes-bip-32-4-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-8-1024.conf -dconfigs/workFile/barnes-bip-32-8-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-16-1024.conf -dconfigs/workFile/barnes-bip-32-16-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-32-1024.conf -dconfigs/workFile/barnes-bip-32-32-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-64-1024.conf -dconfigs/workFile/barnes-bip-32-64-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-128-1024.conf -dconfigs/workFile/barnes-bip-32-128-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-128-1024.$DATE.$HOSTNAME
