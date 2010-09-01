#!/bin/bash

for oldFile in $(ls genome__64_1_8_4_Perfect.conf_*)
do
   configIndex=${oldFile:0-2:2}
   if [[ ${configIndex:0:1} == _ ]]
   then
      configIndex=0${configIndex:1:1}
   fi
   processorSkew2=$(cat $oldFile | grep 'ProcessorSkew\[2\]')
   processorSkew3=$(cat $oldFile | grep 'ProcessorSkew\[3\]')
   sed -i\
      -e "11i$processorSkew2" \
      -e "11i$processorSkew3" \
      fft-004-0001-0002-$configIndex.conf
done
