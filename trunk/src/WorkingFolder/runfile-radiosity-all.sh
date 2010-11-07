#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-moesi-002-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 2 -batch -room &> console-outputs/radiosity-dir-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-msi-002-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 2 -batch -room &> console-outputs/radiosity-dir-msi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-moesi-002-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-moesi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 2 -batch -room &> console-outputs/radiosity-3sd-moesi-002-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-msi-002-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-msi-002-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 2 -batch -room &> console-outputs/radiosity-3sd-msi-002-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-moesi-004-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 4 -batch -room &> console-outputs/radiosity-dir-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-msi-004-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 4 -batch -room &> console-outputs/radiosity-dir-msi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-moesi-004-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-moesi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 4 -batch -room &> console-outputs/radiosity-3sd-moesi-004-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-msi-004-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-msi-004-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 4 -batch -room &> console-outputs/radiosity-3sd-msi-004-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-moesi-008-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 8 -batch -room &> console-outputs/radiosity-dir-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-msi-008-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 8 -batch -room &> console-outputs/radiosity-dir-msi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-moesi-008-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-moesi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 8 -batch -room &> console-outputs/radiosity-3sd-moesi-008-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-msi-008-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-msi-008-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 8 -batch -room &> console-outputs/radiosity-3sd-msi-008-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-moesi-016-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 16 -batch -room &> console-outputs/radiosity-dir-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-msi-016-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 16 -batch -room &> console-outputs/radiosity-dir-msi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-moesi-016-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-moesi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 16 -batch -room &> console-outputs/radiosity-3sd-moesi-016-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-msi-016-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-msi-016-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 16 -batch -room &> console-outputs/radiosity-3sd-msi-016-0001-0002-00.out.$HOSTNAME

nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-moesi-032-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 32 -batch -room &> console-outputs/radiosity-dir-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-dir-msi-032-0001-0002-00.conf -dconfigs/workFile/radiosity-dir-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 32 -batch -room &> console-outputs/radiosity-dir-msi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-moesi-032-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-moesi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 32 -batch -room &> console-outputs/radiosity-3sd-moesi-032-0001-0002-00.out.$HOSTNAME
nice -10 ./augSesc -cconfigs/workFile/radiosity-3sd-msi-032-0001-0002-00.conf -dconfigs/workFile/radiosity-3sd-msi-032-0001-0002-00.conf.report benchmarks-splash2-sesc//radiosity.mips -p 32 -batch -room &> console-outputs/radiosity-3sd-msi-032-0001-0002-00.out.$HOSTNAME

