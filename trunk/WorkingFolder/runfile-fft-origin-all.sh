#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-1-1024.conf -dconfigs/workFile/fft-origin-2-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-1-1024.conf -dconfigs/workFile/fft-origin-4-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-1-1024.conf -dconfigs/workFile/fft-origin-8-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-1-1024.conf -dconfigs/workFile/fft-origin-16-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-1-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-1-1024.conf -dconfigs/workFile/fft-origin-32-1-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-1-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-2-1024.conf -dconfigs/workFile/fft-origin-2-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-2-1024.conf -dconfigs/workFile/fft-origin-4-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-2-1024.conf -dconfigs/workFile/fft-origin-8-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-2-1024.conf -dconfigs/workFile/fft-origin-16-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-2-1024.conf -dconfigs/workFile/fft-origin-32-2-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-4-1024.conf -dconfigs/workFile/fft-origin-2-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-4-1024.conf -dconfigs/workFile/fft-origin-4-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-4-1024.conf -dconfigs/workFile/fft-origin-8-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-4-1024.conf -dconfigs/workFile/fft-origin-16-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-4-1024.conf -dconfigs/workFile/fft-origin-32-4-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-8-1024.conf -dconfigs/workFile/fft-origin-2-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-8-1024.conf -dconfigs/workFile/fft-origin-4-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-8-1024.conf -dconfigs/workFile/fft-origin-8-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-8-1024.conf -dconfigs/workFile/fft-origin-16-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-8-1024.conf -dconfigs/workFile/fft-origin-32-8-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-16-1024.conf -dconfigs/workFile/fft-origin-2-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-16-1024.conf -dconfigs/workFile/fft-origin-4-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-16-1024.conf -dconfigs/workFile/fft-origin-8-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-16-1024.conf -dconfigs/workFile/fft-origin-16-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-16-1024.conf -dconfigs/workFile/fft-origin-32-16-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-32-1024.conf -dconfigs/workFile/fft-origin-2-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-32-1024.conf -dconfigs/workFile/fft-origin-4-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-32-1024.conf -dconfigs/workFile/fft-origin-8-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-32-1024.conf -dconfigs/workFile/fft-origin-16-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-32-1024.conf -dconfigs/workFile/fft-origin-32-32-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-64-1024.conf -dconfigs/workFile/fft-origin-2-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-64-1024.conf -dconfigs/workFile/fft-origin-4-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-64-1024.conf -dconfigs/workFile/fft-origin-8-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-64-1024.conf -dconfigs/workFile/fft-origin-16-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-64-1024.conf -dconfigs/workFile/fft-origin-32-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-2-128-1024.conf -dconfigs/workFile/fft-origin-2-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p2 -n65536 -l4 -t &> console-outputs/fft-origin-2-128-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-4-128-1024.conf -dconfigs/workFile/fft-origin-4-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-origin-4-128-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-8-128-1024.conf -dconfigs/workFile/fft-origin-8-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p8 -n65536 -l4 -t &> console-outputs/fft-origin-8-128-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-16-128-1024.conf -dconfigs/workFile/fft-origin-16-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p16 -n65536 -l4 -t &> console-outputs/fft-origin-16-128-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-128-1024.conf -dconfigs/workFile/fft-origin-32-128-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t &> console-outputs/fft-origin-32-128-1024.$DATE.$HOSTNAME
