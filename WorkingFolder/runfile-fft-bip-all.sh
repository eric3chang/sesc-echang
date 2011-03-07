#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-1-1024.conf -dconfigs/workFile/fft-bip-2-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-2-1024.conf -dconfigs/workFile/fft-bip-2-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-4-1024.conf -dconfigs/workFile/fft-bip-2-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-8-1024.conf -dconfigs/workFile/fft-bip-2-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-16-1024.conf -dconfigs/workFile/fft-bip-2-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-32-1024.conf -dconfigs/workFile/fft-bip-2-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-64-1024.conf -dconfigs/workFile/fft-bip-2-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-2-128-1024.conf -dconfigs/workFile/fft-bip-2-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-bip-2-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-1-1024.conf -dconfigs/workFile/fft-bip-4-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-2-1024.conf -dconfigs/workFile/fft-bip-4-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-4-1024.conf -dconfigs/workFile/fft-bip-4-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-8-1024.conf -dconfigs/workFile/fft-bip-4-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-16-1024.conf -dconfigs/workFile/fft-bip-4-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-32-1024.conf -dconfigs/workFile/fft-bip-4-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-64-1024.conf -dconfigs/workFile/fft-bip-4-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-128-1024.conf -dconfigs/workFile/fft-bip-4-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-1-1024.conf -dconfigs/workFile/fft-bip-8-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-2-1024.conf -dconfigs/workFile/fft-bip-8-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-4-1024.conf -dconfigs/workFile/fft-bip-8-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-8-1024.conf -dconfigs/workFile/fft-bip-8-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-16-1024.conf -dconfigs/workFile/fft-bip-8-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-32-1024.conf -dconfigs/workFile/fft-bip-8-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-64-1024.conf -dconfigs/workFile/fft-bip-8-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-8-128-1024.conf -dconfigs/workFile/fft-bip-8-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-bip-8-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-1-1024.conf -dconfigs/workFile/fft-bip-16-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-2-1024.conf -dconfigs/workFile/fft-bip-16-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-4-1024.conf -dconfigs/workFile/fft-bip-16-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-8-1024.conf -dconfigs/workFile/fft-bip-16-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-16-1024.conf -dconfigs/workFile/fft-bip-16-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-32-1024.conf -dconfigs/workFile/fft-bip-16-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-64-1024.conf -dconfigs/workFile/fft-bip-16-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-16-128-1024.conf -dconfigs/workFile/fft-bip-16-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-bip-16-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-1-1024.conf -dconfigs/workFile/fft-bip-32-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-2-1024.conf -dconfigs/workFile/fft-bip-32-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-4-1024.conf -dconfigs/workFile/fft-bip-32-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-8-1024.conf -dconfigs/workFile/fft-bip-32-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-16-1024.conf -dconfigs/workFile/fft-bip-32-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-32-1024.conf -dconfigs/workFile/fft-bip-32-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-64-1024.conf -dconfigs/workFile/fft-bip-32-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-32-128-1024.conf -dconfigs/workFile/fft-bip-32-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-bip-32-128-1024.$DATE.$HOSTNAME
