#!/bin/bash
for file in $(ls sesc*.c); do
   mipseb-linux-g++ -mips2 -mabi=32 -Wa,-non_shared -mno-abicalls $file -c
done
