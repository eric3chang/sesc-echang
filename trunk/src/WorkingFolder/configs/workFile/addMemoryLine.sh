#!/bin/bash
HOSTNAME=$(hostname)

# already tested
for file in $(ls genome-02cpu-??.conf)
do
   insertString='MemorySystemConfig = "memgen/memoryConfigs/Directory_p2_c1L1-2L2.memory"'
   echo "${insertString}" >> $file
   echo "<base_02cpu.conf>" >> $file
   # add a newline at the end
   echo "" >> $file
done


