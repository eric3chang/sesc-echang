#!/bin/bash

for oldFile in $(ls genome__64_1_8_8_Perfect.conf_*)
do
   configIndex=${oldFile:0-2:2}
   if [[ ${configIndex:0:1} == _ ]]
   then
      configIndex=0${configIndex:1:1}
   fi
   processorSkew4=$(cat $oldFile | grep 'ProcessorSkew\[4\]')
   processorSkew5=$(cat $oldFile | grep 'ProcessorSkew\[5\]')
   processorSkew6=$(cat $oldFile | grep 'ProcessorSkew\[6\]')
   processorSkew7=$(cat $oldFile | grep 'ProcessorSkew\[7\]')
   sed \
      -e "11i$processorSkew4" \
      -e "11i$processorSkew5" \
      -e "11i$processorSkew6" \
      -e "11i$processorSkew7" \
      fft-008-0001-0002-$configIndex.conf
done
