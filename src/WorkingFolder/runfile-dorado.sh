#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

nice -10 ./$AUGSESC -cconfigs/workFile/fft-origin-32-64-128.conf -dconfigs/workFile/fft-origin-32-64-128.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p32 -n65536 -l4 -t
