#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/radix-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/radix-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-msi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/radix-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/radix-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-msi-002-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/radix-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/radix-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-msi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/radix-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/radix-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-msi-004-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/radix-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/radix-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-msi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/radix-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/radix-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-msi-008-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/radix-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/radix-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-msi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/radix-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/radix-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-msi-016-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/radix-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/radix-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-dir-msi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/radix-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc-debug -cconfigs/workFile/radix-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/radix-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-3sd-msi-032-0001-0002-00.out.$HOSTNAME

