#!/bin/bash
HOSTNAME=$(hostname)

# already tested
for i in {1..14}
do
   # if the number is single digit, append 0
   if [[ ${#i} == 1 ]]
   then
      number=0$i
   else
      number=$i
   fi
   insertString="./augSesc -w1 -cconfigs/workFile/genome-02cpu-$number.conf -dconfigs/workFile/genome-02cpu-$number.conf.report benchmarks/genome -t2 -g256 -s16 -n16384 &> console-outputs/genome-02cpu-$number.out.\$HOSTNAME"
   echo "${insertString}" >> newfile.sh
done


