#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/fft-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-dir-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/fft-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-dir-msi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/fft-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/fft-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p2 -n65536 -l4 -t &> console-outputs/fft-3sd-msi-002-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/fft-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p4 -n65536 -l4 -t &> console-outputs/fft-dir-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/fft-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p4 -n65536 -l4 -t &> console-outputs/fft-dir-msi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/fft-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p4 -n65536 -l4 -t &> console-outputs/fft-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/fft-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p4 -n65536 -l4 -t &> console-outputs/fft-3sd-msi-004-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/fft-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p8 -n65536 -l4 -t &> console-outputs/fft-dir-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/fft-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p8 -n65536 -l4 -t &> console-outputs/fft-dir-msi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/fft-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p8 -n65536 -l4 -t &> console-outputs/fft-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/fft-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p8 -n65536 -l4 -t &> console-outputs/fft-3sd-msi-008-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/fft-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-dir-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/fft-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-dir-msi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/fft-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/fft-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p16 -n65536 -l4 -t &> console-outputs/fft-3sd-msi-016-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/fft-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p32 -n65536 -l4 -t &> console-outputs/fft-dir-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/fft-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p32 -n65536 -l4 -t &> console-outputs/fft-dir-msi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/fft-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p32 -n65536 -l4 -t &> console-outputs/fft-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/fft-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/fft-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fft.mips -m10 -p32 -n65536 -l4 -t &> console-outputs/fft-3sd-msi-032-0001-0002-00.out.$HOSTNAME

