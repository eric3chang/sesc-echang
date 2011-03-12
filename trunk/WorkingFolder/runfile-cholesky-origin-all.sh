#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-O1

nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-4-8-1024.conf -dconfigs/workFile/cholesky-origin-4-8-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p4 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-4-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-8-8-1024.conf -dconfigs/workFile/cholesky-origin-8-8-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p8 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-8-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-16-8-1024.conf -dconfigs/workFile/cholesky-origin-16-8-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p16 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-16-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-32-8-1024.conf -dconfigs/workFile/cholesky-origin-32-8-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p32 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-32-8-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-4-16-1024.conf -dconfigs/workFile/cholesky-origin-4-16-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p4 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-4-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-8-16-1024.conf -dconfigs/workFile/cholesky-origin-8-16-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p8 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-8-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-16-16-1024.conf -dconfigs/workFile/cholesky-origin-16-16-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p16 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-16-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-32-16-1024.conf -dconfigs/workFile/cholesky-origin-32-16-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p32 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-32-16-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-4-32-1024.conf -dconfigs/workFile/cholesky-origin-4-32-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p4 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-4-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-8-32-1024.conf -dconfigs/workFile/cholesky-origin-8-32-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p8 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-8-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-16-32-1024.conf -dconfigs/workFile/cholesky-origin-16-32-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p16 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-16-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-32-32-1024.conf -dconfigs/workFile/cholesky-origin-32-32-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p32 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-32-32-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-4-64-1024.conf -dconfigs/workFile/cholesky-origin-4-64-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p4 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-4-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-8-64-1024.conf -dconfigs/workFile/cholesky-origin-8-64-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p8 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-8-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-16-64-1024.conf -dconfigs/workFile/cholesky-origin-16-64-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p16 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-16-64-1024.$DATE.$HOSTNAME
nice -10 ./$AUGSESC -cconfigs/workFile/cholesky-origin-32-64-1024.conf -dconfigs/workFile/cholesky-origin-32-64-1024.conf.report benchmarks-splash2-sesc/cholesky.mips -p32 -B32 -C16384 -t < benchmarks-splash2-sesc/cholesky-inputs/tk23.O &> console-outputs/cholesky-origin-32-64-1024.$DATE.$HOSTNAME
