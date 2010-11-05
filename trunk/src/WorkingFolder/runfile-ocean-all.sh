#!/bin/bash
HOSTNAME=$(hostname)

./augSesc -cconfigs/workFile/ocean-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/ocean-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/ocean-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-msi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-msi-002-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/ocean-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/ocean-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/ocean-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-msi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-msi-004-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/ocean-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/ocean-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/ocean-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-msi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-msi-008-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/ocean-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/ocean-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/ocean-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-msi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-msi-016-0001-0002-00.out.$HOSTNAME

./augSesc -cconfigs/workFile/ocean-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/ocean-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/ocean-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-dir-msi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
./augSesc -cconfigs/workFile/ocean-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/ocean-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-3sd-msi-032-0001-0002-00.out.$HOSTNAME

