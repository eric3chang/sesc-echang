#!/bin/bash
HOSTNAME=$(hostname)

./augSesc -cconfigs/workFile/fmm-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/fmm-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu2   &> console-outputs/fmm-dir-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/fmm-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu2   &> console-outputs/fmm-dir-msi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu2   &> console-outputs/fmm-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu2   &> console-outputs/fmm-3sd-msi-002-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/fmm-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/fmm-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu4   &> console-outputs/fmm-dir-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/fmm-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu4   &> console-outputs/fmm-dir-msi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu4   &> console-outputs/fmm-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu4   &> console-outputs/fmm-3sd-msi-004-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/fmm-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/fmm-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu8   &> console-outputs/fmm-dir-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/fmm-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu8   &> console-outputs/fmm-dir-msi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu8   &> console-outputs/fmm-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu8   &> console-outputs/fmm-3sd-msi-008-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/fmm-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/fmm-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu16   &> console-outputs/fmm-dir-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/fmm-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu16   &> console-outputs/fmm-dir-msi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu16   &> console-outputs/fmm-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu16   &> console-outputs/fmm-3sd-msi-016-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/fmm-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/fmm-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu32   &> console-outputs/fmm-dir-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/fmm-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu32   &> console-outputs/fmm-dir-msi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu32   &> console-outputs/fmm-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/fmm-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/fmm-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//fmm.mips   -o < benchmarks-splash2-sesc/fmm-inputs/cpu32   &> console-outputs/fmm-3sd-msi-032-0001-0002-00.out.$HOSTNAME

