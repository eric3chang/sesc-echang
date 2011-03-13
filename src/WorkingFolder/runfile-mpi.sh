#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

nice -10 ./$AUGSESC -cconfigs/workFile/fft-bip-4-64-1024.conf -dconfigs/workFile/fft-bip-4-64-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p4 -n65536 -l4 -t &> console-outputs/fft-bip-4-64-1024.$DATE.$HOSTNAME
