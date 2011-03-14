#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-O1

nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-4-64-128.conf -dconfigs/workFile/lu-bip-4-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b32 -t &> console-outputs/lu-bip-4-64-128.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-4-64-256.conf -dconfigs/workFile/lu-bip-4-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b32 -t &> console-outputs/lu-bip-4-64-256.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-4-64-512.conf -dconfigs/workFile/lu-bip-4-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b32 -t &> console-outputs/lu-bip-4-64-512.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-4-64-1024.conf -dconfigs/workFile/lu-bip-4-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b32 -t &> console-outputs/lu-bip-4-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-4-64-2048.conf -dconfigs/workFile/lu-bip-4-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b32 -t &> console-outputs/lu-bip-4-64-2048.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-4-64-4096.conf -dconfigs/workFile/lu-bip-4-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p4 -b32 -t &> console-outputs/lu-bip-4-64-4096.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-8-64-128.conf -dconfigs/workFile/lu-bip-8-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b32 -t &> console-outputs/lu-bip-8-64-128.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-8-64-256.conf -dconfigs/workFile/lu-bip-8-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b32 -t &> console-outputs/lu-bip-8-64-256.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-8-64-512.conf -dconfigs/workFile/lu-bip-8-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b32 -t &> console-outputs/lu-bip-8-64-512.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-8-64-1024.conf -dconfigs/workFile/lu-bip-8-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b32 -t &> console-outputs/lu-bip-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-8-64-2048.conf -dconfigs/workFile/lu-bip-8-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b32 -t &> console-outputs/lu-bip-8-64-2048.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-8-64-4096.conf -dconfigs/workFile/lu-bip-8-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p8 -b32 -t &> console-outputs/lu-bip-8-64-4096.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-16-64-128.conf -dconfigs/workFile/lu-bip-16-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b32 -t &> console-outputs/lu-bip-16-64-128.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-16-64-256.conf -dconfigs/workFile/lu-bip-16-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b32 -t &> console-outputs/lu-bip-16-64-256.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-16-64-512.conf -dconfigs/workFile/lu-bip-16-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b32 -t &> console-outputs/lu-bip-16-64-512.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-16-64-1024.conf -dconfigs/workFile/lu-bip-16-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b32 -t &> console-outputs/lu-bip-16-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-16-64-2048.conf -dconfigs/workFile/lu-bip-16-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b32 -t &> console-outputs/lu-bip-16-64-2048.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-16-64-4096.conf -dconfigs/workFile/lu-bip-16-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p16 -b32 -t &> console-outputs/lu-bip-16-64-4096.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-32-64-128.conf -dconfigs/workFile/lu-bip-32-64-128.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b32 -t &> console-outputs/lu-bip-32-64-128.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-32-64-256.conf -dconfigs/workFile/lu-bip-32-64-256.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b32 -t &> console-outputs/lu-bip-32-64-256.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-32-64-512.conf -dconfigs/workFile/lu-bip-32-64-512.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b32 -t &> console-outputs/lu-bip-32-64-512.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-32-64-1024.conf -dconfigs/workFile/lu-bip-32-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b32 -t &> console-outputs/lu-bip-32-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-32-64-2048.conf -dconfigs/workFile/lu-bip-32-64-2048.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b32 -t &> console-outputs/lu-bip-32-64-2048.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/lu-bip-32-64-4096.conf -dconfigs/workFile/lu-bip-32-64-4096.conf.report benchmarks-splash2-sesc/lu.mips -n256 -p32 -b32 -t &> console-outputs/lu-bip-32-64-4096.$DATE.$HOSTNAME
