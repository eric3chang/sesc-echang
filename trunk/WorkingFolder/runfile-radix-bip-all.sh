#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Release

nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-2-8-1024.conf -dconfigs/workFile/radix-bip-2-8-1024.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-2-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-2-16-1024.conf -dconfigs/workFile/radix-bip-2-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-2-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-2-32-1024.conf -dconfigs/workFile/radix-bip-2-32-1024.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-2-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-2-64-1024.conf -dconfigs/workFile/radix-bip-2-64-1024.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-2-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-2-128-1024.conf -dconfigs/workFile/radix-bip-2-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-2-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-4-8-1024.conf -dconfigs/workFile/radix-bip-4-8-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-4-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-4-16-1024.conf -dconfigs/workFile/radix-bip-4-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-4-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-4-32-1024.conf -dconfigs/workFile/radix-bip-4-32-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-4-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-4-64-1024.conf -dconfigs/workFile/radix-bip-4-64-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-4-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-4-128-1024.conf -dconfigs/workFile/radix-bip-4-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-4-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-8-8-1024.conf -dconfigs/workFile/radix-bip-8-8-1024.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-8-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-8-16-1024.conf -dconfigs/workFile/radix-bip-8-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-8-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-8-32-1024.conf -dconfigs/workFile/radix-bip-8-32-1024.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-8-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-8-64-1024.conf -dconfigs/workFile/radix-bip-8-64-1024.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-8-128-1024.conf -dconfigs/workFile/radix-bip-8-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-8-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-16-8-1024.conf -dconfigs/workFile/radix-bip-16-8-1024.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-16-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-16-16-1024.conf -dconfigs/workFile/radix-bip-16-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-16-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-16-32-1024.conf -dconfigs/workFile/radix-bip-16-32-1024.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-16-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-16-64-1024.conf -dconfigs/workFile/radix-bip-16-64-1024.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-16-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-16-128-1024.conf -dconfigs/workFile/radix-bip-16-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-16-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-32-8-1024.conf -dconfigs/workFile/radix-bip-32-8-1024.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-32-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-32-16-1024.conf -dconfigs/workFile/radix-bip-32-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-32-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-32-32-1024.conf -dconfigs/workFile/radix-bip-32-32-1024.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-32-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-32-64-1024.conf -dconfigs/workFile/radix-bip-32-64-1024.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-32-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/radix-bip-32-128-1024.conf -dconfigs/workFile/radix-bip-32-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n524288 -r1024 -m524288 &> console-outputs/radix-bip-32-128-1024.$DATE.$HOSTNAME
