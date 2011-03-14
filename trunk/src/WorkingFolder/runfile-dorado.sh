#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-O1

nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-4-64-128.conf -dconfigs/workFile/lu-origin-4-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b8 -t &> console-outputs/lu-origin-4-64-128.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-4-64-256.conf -dconfigs/workFile/lu-origin-4-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b8 -t &> console-outputs/lu-origin-4-64-256.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-4-64-512.conf -dconfigs/workFile/lu-origin-4-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b8 -t &> console-outputs/lu-origin-4-64-512.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-4-64-1024.conf -dconfigs/workFile/lu-origin-4-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b8 -t &> console-outputs/lu-origin-4-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-4-64-2048.conf -dconfigs/workFile/lu-origin-4-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b8 -t &> console-outputs/lu-origin-4-64-2048.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-4-64-4096.conf -dconfigs/workFile/lu-origin-4-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b8 -t &> console-outputs/lu-origin-4-64-4096.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-8-64-128.conf -dconfigs/workFile/lu-origin-8-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b8 -t &> console-outputs/lu-origin-8-64-128.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-8-64-256.conf -dconfigs/workFile/lu-origin-8-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b8 -t &> console-outputs/lu-origin-8-64-256.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-8-64-512.conf -dconfigs/workFile/lu-origin-8-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b8 -t &> console-outputs/lu-origin-8-64-512.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-8-64-1024.conf -dconfigs/workFile/lu-origin-8-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b8 -t &> console-outputs/lu-origin-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-8-64-2048.conf -dconfigs/workFile/lu-origin-8-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b8 -t &> console-outputs/lu-origin-8-64-2048.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-8-64-4096.conf -dconfigs/workFile/lu-origin-8-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b8 -t &> console-outputs/lu-origin-8-64-4096.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-16-64-128.conf -dconfigs/workFile/lu-origin-16-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b8 -t &> console-outputs/lu-origin-16-64-128.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-16-64-256.conf -dconfigs/workFile/lu-origin-16-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b8 -t &> console-outputs/lu-origin-16-64-256.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-16-64-512.conf -dconfigs/workFile/lu-origin-16-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b8 -t &> console-outputs/lu-origin-16-64-512.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-16-64-1024.conf -dconfigs/workFile/lu-origin-16-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b8 -t &> console-outputs/lu-origin-16-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-16-64-2048.conf -dconfigs/workFile/lu-origin-16-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b8 -t &> console-outputs/lu-origin-16-64-2048.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-16-64-4096.conf -dconfigs/workFile/lu-origin-16-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b8 -t &> console-outputs/lu-origin-16-64-4096.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-32-64-128.conf -dconfigs/workFile/lu-origin-32-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b8 -t &> console-outputs/lu-origin-32-64-128.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-32-64-256.conf -dconfigs/workFile/lu-origin-32-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b8 -t &> console-outputs/lu-origin-32-64-256.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-32-64-512.conf -dconfigs/workFile/lu-origin-32-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b8 -t &> console-outputs/lu-origin-32-64-512.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-32-64-1024.conf -dconfigs/workFile/lu-origin-32-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b8 -t &> console-outputs/lu-origin-32-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-32-64-2048.conf -dconfigs/workFile/lu-origin-32-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b8 -t &> console-outputs/lu-origin-32-64-2048.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-origin-32-64-4096.conf -dconfigs/workFile/lu-origin-32-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b8 -t &> console-outputs/lu-origin-32-64-4096.$DATE.$HOSTNAME
