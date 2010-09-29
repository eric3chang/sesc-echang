#!/bin/bash
HOSTNAME=$(hostname)

./augSesc-non-verbose  -cconfigs/workFile/fft-002-0001-0002-00.conf -dconfigs/workFile/fft-002-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-002-0001-0002-00.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-004-0001-0002-00.conf -dconfigs/workFile/fft-004-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p4 -n65536 -l4 -t &> console-outputs/fft-004-0001-0002-00.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-008-0001-0002-00.conf -dconfigs/workFile/fft-008-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p8 -n65536 -l4 -t &> console-outputs/fft-008-0001-0002-00.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-snoopy-moesi-002-0001-0002-00.conf -dconfigs/workFile/fft-snoopy-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-snoopy-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-snoopy-moesi-004-0001-0002-00.conf -dconfigs/workFile/fft-snoopy-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p4 -n65536 -l4 -t &> console-outputs/fft-snoopy-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc-non-verbose  -cconfigs/workFile/fft-snoopy-moesi-008-0001-0002-00.conf -dconfigs/workFile/fft-snoopy-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc/fft.mips -m10 -p8 -n65536 -l4 -t &> console-outputs/fft-snoopy-moesi-008-0001-0002-00.out.$HOSTNAME
