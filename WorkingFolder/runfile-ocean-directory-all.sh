#!/bin/bash
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-2-16-1024.conf -dconfigs/workFile/ocean-directory-2-16-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-2-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-2-32-1024.conf -dconfigs/workFile/ocean-directory-2-32-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-2-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-2-64-1024.conf -dconfigs/workFile/ocean-directory-2-64-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-2-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-2-128-1024.conf -dconfigs/workFile/ocean-directory-2-128-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p2 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-2-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-4-16-1024.conf -dconfigs/workFile/ocean-directory-4-16-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-4-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-4-32-1024.conf -dconfigs/workFile/ocean-directory-4-32-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-4-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-4-64-1024.conf -dconfigs/workFile/ocean-directory-4-64-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-4-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-4-128-1024.conf -dconfigs/workFile/ocean-directory-4-128-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p4 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-4-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-8-16-1024.conf -dconfigs/workFile/ocean-directory-8-16-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-8-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-8-32-1024.conf -dconfigs/workFile/ocean-directory-8-32-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-8-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-8-64-1024.conf -dconfigs/workFile/ocean-directory-8-64-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-8-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-8-128-1024.conf -dconfigs/workFile/ocean-directory-8-128-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p8 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-8-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-16-16-1024.conf -dconfigs/workFile/ocean-directory-16-16-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-16-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-16-32-1024.conf -dconfigs/workFile/ocean-directory-16-32-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-16-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-16-64-1024.conf -dconfigs/workFile/ocean-directory-16-64-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-16-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-16-128-1024.conf -dconfigs/workFile/ocean-directory-16-128-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p16 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-16-128-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-32-16-1024.conf -dconfigs/workFile/ocean-directory-32-16-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-32-16-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-32-32-1024.conf -dconfigs/workFile/ocean-directory-32-32-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-32-32-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-32-64-1024.conf -dconfigs/workFile/ocean-directory-32-64-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-32-64-1024.out.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/ocean-directory-32-128-1024.conf -dconfigs/workFile/ocean-directory-32-128-1024.conf.report benchmarks-splash2-sesc/ocean.mips -n130 -p32 -e1e-7 -r20000.0 -t28800.0 &> console-outputs/ocean-directory-32-128-1024.out.$HOSTNAME