#!/bin/bash
# script to help compile custom programs

FILE=$1  # uses the first passed-in parameter for filename
SESC_THREAD_O=~/csl/sesc-echang/src/libapp/sesc_thread.o
MINT_X=/opt/csl/sescutils/mipseb-linux/lib/ldscripts/mint.x
EXTENSION=sesc

if [[ $# != 1 ]]; then
   echo "usage: compile-sesc.sh [YOUR FILE]"
   echo "sesc binary will be generated with the extension .$EXTENSION"
   exit 1
fi

mipseb-linux-gcc -o "${FILE%.*}.$EXTENSION" "${FILE}" -g -mips2 -mabi=32 -static -Wa,-non_shared -mno-abicalls "$SESC_THREAD_O" -Wl,--script="$MINT_X"

