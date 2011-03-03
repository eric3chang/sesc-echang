#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-16-1024.conf -dconfigs/workFile/lu-bip-2-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-32-1024.conf -dconfigs/workFile/lu-bip-2-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-64-1024.conf -dconfigs/workFile/lu-bip-2-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-128-1024.conf -dconfigs/workFile/lu-bip-2-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-16-1024.conf -dconfigs/workFile/lu-bip-4-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-32-1024.conf -dconfigs/workFile/lu-bip-4-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-64-1024.conf -dconfigs/workFile/lu-bip-4-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-128-1024.conf -dconfigs/workFile/lu-bip-4-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-16-1024.conf -dconfigs/workFile/lu-bip-8-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-32-1024.conf -dconfigs/workFile/lu-bip-8-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-64-1024.conf -dconfigs/workFile/lu-bip-8-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-128-1024.conf -dconfigs/workFile/lu-bip-8-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-16-1024.conf -dconfigs/workFile/lu-bip-16-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-32-1024.conf -dconfigs/workFile/lu-bip-16-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-64-1024.conf -dconfigs/workFile/lu-bip-16-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-128-1024.conf -dconfigs/workFile/lu-bip-16-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-16-1024.conf -dconfigs/workFile/lu-bip-32-16-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-16-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-32-1024.conf -dconfigs/workFile/lu-bip-32-32-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-32-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-64-1024.conf -dconfigs/workFile/lu-bip-32-64-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-64-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-128-1024.conf -dconfigs/workFile/lu-bip-32-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-128-1024.$DATE.$HOSTNAME
