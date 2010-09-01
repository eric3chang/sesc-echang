#!/bin/bash
# add more processor skews to config files

OLD_CPU_CNT=4
NEW_CPU_CNT=8

for oldFile in $(ls genome__64_1_8_${NEW_CPU_CNT}_Perfect.conf_*)
do
   configIndex=${oldFile:0-2:2}
   if [[ ${configIndex:0:1} == _ ]]
   then
      configIndex=0${configIndex:1:1}
   fi
   index=$(($NEW_CPU_CNT-1))
   insertPoint=$(($OLD_CPU_CNT+9))
   while [[ $index -ge $OLD_CPU_CNT ]]
   do
      processorSkew=$(cat $oldFile | grep "ProcessorSkew\[$index\]")
      sed -i\
         -e "${insertPoint}i$processorSkew" \
         fft-00$NEW_CPU_CNT-0001-0002-$configIndex.conf
      let index--
   done
done
