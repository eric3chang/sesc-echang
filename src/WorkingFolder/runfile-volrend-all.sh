#!/bin/bash
HOSTNAME=$(hostname)

./augSesc -cconfigs/workFile/volrend-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/volrend-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  2 head-scaleddown4 &> console-outputs/volrend-dir-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/volrend-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  2 head-scaleddown4 &> console-outputs/volrend-dir-msi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  2 head-scaleddown4 &> console-outputs/volrend-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  2 head-scaleddown4 &> console-outputs/volrend-3sd-msi-002-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/volrend-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/volrend-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  4 head-scaleddown4 &> console-outputs/volrend-dir-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/volrend-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  4 head-scaleddown4 &> console-outputs/volrend-dir-msi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  4 head-scaleddown4 &> console-outputs/volrend-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  4 head-scaleddown4 &> console-outputs/volrend-3sd-msi-004-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/volrend-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/volrend-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  8 head-scaleddown4 &> console-outputs/volrend-dir-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/volrend-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  8 head-scaleddown4 &> console-outputs/volrend-dir-msi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  8 head-scaleddown4 &> console-outputs/volrend-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  8 head-scaleddown4 &> console-outputs/volrend-3sd-msi-008-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/volrend-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/volrend-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  16 head-scaleddown4 &> console-outputs/volrend-dir-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/volrend-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  16 head-scaleddown4 &> console-outputs/volrend-dir-msi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  16 head-scaleddown4 &> console-outputs/volrend-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  16 head-scaleddown4 &> console-outputs/volrend-3sd-msi-016-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/volrend-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/volrend-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  32 head-scaleddown4 &> console-outputs/volrend-dir-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/volrend-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  32 head-scaleddown4 &> console-outputs/volrend-dir-msi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  32 head-scaleddown4 &> console-outputs/volrend-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/volrend-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/volrend-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//volrend.mips  32 head-scaleddown4 &> console-outputs/volrend-3sd-msi-032-0001-0002-00.out.$HOSTNAME

