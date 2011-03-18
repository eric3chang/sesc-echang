#!/bin/bash
# script to help compile custom programs

FILE=$1  # uses the first passed-in parameter for filename
#SESC_THREAD_O=~/csl/sesc-echang/src/libapp/sesc_thread.o
OBJECT_FILES="sesc_chud.o sesc_events.o sesc_locks.o sesc_thread.o sesc_tls.o sesc_ts.o"
OUT_DIR='../WorkingFolder/benchmarks-splash2-sesc/'
#SESC_THREAD_O=sesc_chud.o\ sesc_locks.o\ sesc_thread.o\ sesc_tls.o\ sesc_ts.o
MINT_X=/opt/csl/sescutils/mipseb-linux/lib/ldscripts/mint.x
EXTENSION=mips

if [[ $# < 1 ]]; then
   echo "usage: compile-sesc.sh [YOUR FILE] [OTHER GCC PARAMETERS]"
   echo "sesc binary will be generated with the extension .$EXTENSION"
   exit 1
fi

shift

mipseb-linux-g++ -o "${OUT_DIR}${FILE%.*}.$EXTENSION" -DUSE_SESC "${FILE}" -g -mips2 -mabi=32 -static -Wa,-non_shared -mno-abicalls $OBJECT_FILES -Wl,--script="$MINT_X" $@

