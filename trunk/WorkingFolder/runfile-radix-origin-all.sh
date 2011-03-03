#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-2-128-512.conf -dconfigs/workFile/radix-origin-2-128-512.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-2-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-2-128-1024.conf -dconfigs/workFile/radix-origin-2-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-2-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-2-128-2048.conf -dconfigs/workFile/radix-origin-2-128-2048.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-2-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-2-128-4096.conf -dconfigs/workFile/radix-origin-2-128-4096.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-2-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-2-128-8192.conf -dconfigs/workFile/radix-origin-2-128-8192.conf.report benchmarks-splash2-sesc/radix.mips -p2 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-2-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-4-128-512.conf -dconfigs/workFile/radix-origin-4-128-512.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-4-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-4-128-1024.conf -dconfigs/workFile/radix-origin-4-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-4-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-4-128-2048.conf -dconfigs/workFile/radix-origin-4-128-2048.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-4-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-4-128-4096.conf -dconfigs/workFile/radix-origin-4-128-4096.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-4-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-4-128-8192.conf -dconfigs/workFile/radix-origin-4-128-8192.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-4-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-8-128-512.conf -dconfigs/workFile/radix-origin-8-128-512.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-8-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-8-128-1024.conf -dconfigs/workFile/radix-origin-8-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-8-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-8-128-2048.conf -dconfigs/workFile/radix-origin-8-128-2048.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-8-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-8-128-4096.conf -dconfigs/workFile/radix-origin-8-128-4096.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-8-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-8-128-8192.conf -dconfigs/workFile/radix-origin-8-128-8192.conf.report benchmarks-splash2-sesc/radix.mips -p8 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-8-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-16-128-512.conf -dconfigs/workFile/radix-origin-16-128-512.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-16-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-16-128-1024.conf -dconfigs/workFile/radix-origin-16-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-16-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-16-128-2048.conf -dconfigs/workFile/radix-origin-16-128-2048.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-16-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-16-128-4096.conf -dconfigs/workFile/radix-origin-16-128-4096.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-16-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-16-128-8192.conf -dconfigs/workFile/radix-origin-16-128-8192.conf.report benchmarks-splash2-sesc/radix.mips -p16 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-16-128-8192.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-32-128-512.conf -dconfigs/workFile/radix-origin-32-128-512.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-32-128-512.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-32-128-1024.conf -dconfigs/workFile/radix-origin-32-128-1024.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-32-128-1024.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-32-128-2048.conf -dconfigs/workFile/radix-origin-32-128-2048.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-32-128-2048.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-32-128-4096.conf -dconfigs/workFile/radix-origin-32-128-4096.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-32-128-4096.$DATE.$HOSTNAME
nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-32-128-8192.conf -dconfigs/workFile/radix-origin-32-128-8192.conf.report benchmarks-splash2-sesc/radix.mips -p32 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-32-128-8192.$DATE.$HOSTNAME
