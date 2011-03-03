#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-128-512.conf -dconfigs/workFile/lu-bip-2-128-512.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-128-1024.conf -dconfigs/workFile/lu-bip-2-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-128-2048.conf -dconfigs/workFile/lu-bip-2-128-2048.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-128-4096.conf -dconfigs/workFile/lu-bip-2-128-4096.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-2-128-8192.conf -dconfigs/workFile/lu-bip-2-128-8192.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p2 -b16 -t &> console-outputs/lu-bip-2-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-128-512.conf -dconfigs/workFile/lu-bip-4-128-512.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-128-1024.conf -dconfigs/workFile/lu-bip-4-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-128-2048.conf -dconfigs/workFile/lu-bip-4-128-2048.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-128-4096.conf -dconfigs/workFile/lu-bip-4-128-4096.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-4-128-8192.conf -dconfigs/workFile/lu-bip-4-128-8192.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p4 -b16 -t &> console-outputs/lu-bip-4-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-128-512.conf -dconfigs/workFile/lu-bip-8-128-512.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-128-1024.conf -dconfigs/workFile/lu-bip-8-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-128-2048.conf -dconfigs/workFile/lu-bip-8-128-2048.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-128-4096.conf -dconfigs/workFile/lu-bip-8-128-4096.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-8-128-8192.conf -dconfigs/workFile/lu-bip-8-128-8192.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p8 -b16 -t &> console-outputs/lu-bip-8-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-128-512.conf -dconfigs/workFile/lu-bip-16-128-512.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-128-1024.conf -dconfigs/workFile/lu-bip-16-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-128-2048.conf -dconfigs/workFile/lu-bip-16-128-2048.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-128-4096.conf -dconfigs/workFile/lu-bip-16-128-4096.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-16-128-8192.conf -dconfigs/workFile/lu-bip-16-128-8192.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p16 -b16 -t &> console-outputs/lu-bip-16-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-128-512.conf -dconfigs/workFile/lu-bip-32-128-512.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-128-1024.conf -dconfigs/workFile/lu-bip-32-128-1024.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-128-2048.conf -dconfigs/workFile/lu-bip-32-128-2048.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-128-4096.conf -dconfigs/workFile/lu-bip-32-128-4096.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/lu-bip-32-128-8192.conf -dconfigs/workFile/lu-bip-32-128-8192.conf.report benchmarks-splash2-sesc/lu.mips -n512 -p32 -b16 -t &> console-outputs/lu-bip-32-128-8192.$DATE.$HOSTNAME
