#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-2-16-1024.conf -dconfigs/workFile/lu-origin-2-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-origin-2-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-2-32-1024.conf -dconfigs/workFile/lu-origin-2-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-origin-2-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-2-64-1024.conf -dconfigs/workFile/lu-origin-2-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-origin-2-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-2-128-1024.conf -dconfigs/workFile/lu-origin-2-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-origin-2-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-4-16-1024.conf -dconfigs/workFile/lu-origin-4-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-origin-4-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-4-32-1024.conf -dconfigs/workFile/lu-origin-4-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-origin-4-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-4-64-1024.conf -dconfigs/workFile/lu-origin-4-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-origin-4-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-4-128-1024.conf -dconfigs/workFile/lu-origin-4-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-origin-4-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-8-16-1024.conf -dconfigs/workFile/lu-origin-8-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-origin-8-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-8-32-1024.conf -dconfigs/workFile/lu-origin-8-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-origin-8-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-8-64-1024.conf -dconfigs/workFile/lu-origin-8-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-origin-8-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-8-128-1024.conf -dconfigs/workFile/lu-origin-8-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-origin-8-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-16-16-1024.conf -dconfigs/workFile/lu-origin-16-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-origin-16-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-16-32-1024.conf -dconfigs/workFile/lu-origin-16-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-origin-16-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-16-64-1024.conf -dconfigs/workFile/lu-origin-16-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-origin-16-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-16-128-1024.conf -dconfigs/workFile/lu-origin-16-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-origin-16-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-32-16-1024.conf -dconfigs/workFile/lu-origin-32-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-origin-32-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-32-32-1024.conf -dconfigs/workFile/lu-origin-32-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-origin-32-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-32-64-1024.conf -dconfigs/workFile/lu-origin-32-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-origin-32-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-origin-32-128-1024.conf -dconfigs/workFile/lu-origin-32-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-origin-32-128-1024.$DATE.$HOSTNAME
