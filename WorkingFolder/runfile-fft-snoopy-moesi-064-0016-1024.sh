#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc-debug  -cconfigs/workFile/fft-snoopy-moesi-064-0016-1024.conf -dconfigs/workFile/fft-snoopy-moesi-064-0016-1024.conf.report benchmarks-splash2-sesc/fft.mips -m12 -p64 -n65536 -l4 -t &> console-outputs/fft-snoopy-moesi-064-0016-1024.out.$HOSTNAME
