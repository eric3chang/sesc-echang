#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-O1

#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-1-1024.conf -dconfigs/workFile/raytrace-bip-2-1-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-1-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-1-1024.conf -dconfigs/workFile/raytrace-bip-4-1-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-1-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-1-1024.conf -dconfigs/workFile/raytrace-bip-8-1-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-1-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-1-1024.conf -dconfigs/workFile/raytrace-bip-16-1-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-1-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-1-1024.conf -dconfigs/workFile/raytrace-bip-32-1-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-1-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-2-1024.conf -dconfigs/workFile/raytrace-bip-2-2-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-2-1024.conf -dconfigs/workFile/raytrace-bip-4-2-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-2-1024.conf -dconfigs/workFile/raytrace-bip-8-2-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-2-1024.conf -dconfigs/workFile/raytrace-bip-16-2-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-2-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-2-1024.conf -dconfigs/workFile/raytrace-bip-32-2-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-2-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-4-1024.conf -dconfigs/workFile/raytrace-bip-2-4-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-4-1024.conf -dconfigs/workFile/raytrace-bip-4-4-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-4-1024.conf -dconfigs/workFile/raytrace-bip-8-4-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-4-1024.conf -dconfigs/workFile/raytrace-bip-16-4-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-4-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-4-1024.conf -dconfigs/workFile/raytrace-bip-32-4-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-4-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-8-1024.conf -dconfigs/workFile/raytrace-bip-2-8-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-8-1024.conf -dconfigs/workFile/raytrace-bip-4-8-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-8-1024.conf -dconfigs/workFile/raytrace-bip-8-8-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-8-1024.conf -dconfigs/workFile/raytrace-bip-16-8-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-8-1024.conf -dconfigs/workFile/raytrace-bip-32-8-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-8-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-16-1024.conf -dconfigs/workFile/raytrace-bip-2-16-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-16-1024.conf -dconfigs/workFile/raytrace-bip-4-16-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-16-1024.conf -dconfigs/workFile/raytrace-bip-8-16-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-16-1024.conf -dconfigs/workFile/raytrace-bip-16-16-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-16-1024.conf -dconfigs/workFile/raytrace-bip-32-16-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-16-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-32-1024.conf -dconfigs/workFile/raytrace-bip-2-32-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-32-1024.conf -dconfigs/workFile/raytrace-bip-4-32-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-32-1024.conf -dconfigs/workFile/raytrace-bip-8-32-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-32-1024.conf -dconfigs/workFile/raytrace-bip-16-32-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-32-1024.conf -dconfigs/workFile/raytrace-bip-32-32-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-32-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-64-1024.conf -dconfigs/workFile/raytrace-bip-2-64-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-64-1024.conf -dconfigs/workFile/raytrace-bip-4-64-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-64-1024.conf -dconfigs/workFile/raytrace-bip-8-64-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-64-1024.conf -dconfigs/workFile/raytrace-bip-16-64-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-64-1024.conf -dconfigs/workFile/raytrace-bip-32-64-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-64-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-2-128-1024.conf -dconfigs/workFile/raytrace-bip-2-128-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p2 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-2-128-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-4-128-1024.conf -dconfigs/workFile/raytrace-bip-4-128-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p4 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-4-128-1024.$DATE.$HOSTNAME
#nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-8-128-1024.conf -dconfigs/workFile/raytrace-bip-8-128-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p8 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-8-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-16-128-1024.conf -dconfigs/workFile/raytrace-bip-16-128-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p16 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-16-128-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/raytrace-bip-32-128-1024.conf -dconfigs/workFile/raytrace-bip-32-128-1024.conf.report benchmarks-splash2-sesc/raytrace.mips -p32 benchmarks-splash2-sesc/raytrace-inputs/balls4.env &> console-outputs/raytrace-bip-32-128-1024.$DATE.$HOSTNAME
