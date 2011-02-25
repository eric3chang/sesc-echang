#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

nice -10 ./augSesc-Debug -cconfigs/workFile/radix-bip-4-16-1024.conf -dconfigs/workFile/radix-bip-4-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-bip-4-16-1024.$DATE.$HOSTNAME

nice -10 ./augSesc-Debug -cconfigs/workFile/radix-origin-4-16-1024.conf -dconfigs/workFile/radix-origin-4-16-1024.conf.report benchmarks-splash2-sesc/radix.mips -p4 -n262144 -r1024 -m524288 &> console-outputs/radix-origin-4-16-1024.$DATE.$HOSTNAME
