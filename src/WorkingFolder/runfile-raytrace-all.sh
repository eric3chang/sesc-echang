#!/bin/bash
HOSTNAME=$(hostname)

./augSesc -cconfigs/workFile/raytrace-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p2 balls4.env &> console-outputs/raytrace-dir-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p2 balls4.env &> console-outputs/raytrace-dir-msi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p2 balls4.env &> console-outputs/raytrace-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p2 balls4.env &> console-outputs/raytrace-3sd-msi-002-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/raytrace-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p4 balls4.env &> console-outputs/raytrace-dir-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p4 balls4.env &> console-outputs/raytrace-dir-msi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p4 balls4.env &> console-outputs/raytrace-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p4 balls4.env &> console-outputs/raytrace-3sd-msi-004-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/raytrace-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p8 balls4.env &> console-outputs/raytrace-dir-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p8 balls4.env &> console-outputs/raytrace-dir-msi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p8 balls4.env &> console-outputs/raytrace-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p8 balls4.env &> console-outputs/raytrace-3sd-msi-008-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/raytrace-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p16 balls4.env &> console-outputs/raytrace-dir-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p16 balls4.env &> console-outputs/raytrace-dir-msi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p16 balls4.env &> console-outputs/raytrace-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p16 balls4.env &> console-outputs/raytrace-3sd-msi-016-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/raytrace-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p32 balls4.env &> console-outputs/raytrace-dir-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/raytrace-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p32 balls4.env &> console-outputs/raytrace-dir-msi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p32 balls4.env &> console-outputs/raytrace-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/raytrace-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/raytrace-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//raytrace.mips -p32 balls4.env &> console-outputs/raytrace-3sd-msi-032-0001-0002-00.out.$HOSTNAME

