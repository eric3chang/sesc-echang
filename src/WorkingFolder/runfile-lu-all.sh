#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc -cconfigs/workFile/lu-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/lu-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-dir-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/lu-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-dir-msi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/lu-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/lu-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-3sd-msi-002-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/lu-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/lu-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-dir-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/lu-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-dir-msi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/lu-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/lu-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-3sd-msi-004-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/lu-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/lu-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-dir-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/lu-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-dir-msi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/lu-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/lu-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-3sd-msi-008-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/lu-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/lu-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-dir-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/lu-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-dir-msi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/lu-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/lu-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-3sd-msi-016-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/lu-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/lu-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-dir-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/lu-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-dir-msi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/lu-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/lu-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/lu-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-3sd-msi-032-0001-0002-00.out.$HOSTNAME

