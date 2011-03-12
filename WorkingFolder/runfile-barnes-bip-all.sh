#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-O1

nice -10 ./$AUGSESC -cconfigs/workFile/barnes-bip-32-1-1024.conf -dconfigs/workFile/barnes-bip-32-1-1024.conf.report benchmarks-splash2-sesc/barnes.mips < benchmarks-splash2-sesc/barnes-inputs/cpu32-special &> console-outputs/barnes-bip-32-1-1024.$DATE.$HOSTNAME
