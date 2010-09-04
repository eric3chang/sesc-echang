#!/bin/bash
# add more processor skews to config files

NEW_FILENAME_PREFIX="fft-3sd-moesi"
OLD_CPU_CNT=4
NEW_CPU_CNT=8

# check for whether we can use dos2unix or fromdos
if [[ -e '/usr/bin/dos2unix' ]]
then
   DOS2UNIX=dos2unix
elif [[ -e '/usr/bin/fromdos' ]]
then
   DOS2UNIX=fromdos
else
   echo "Cannot find dos2unix or fromdos, not converting file to unix format"
fi

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
      newFilename=${NEW_FILENAME_PREFIX}-00$NEW_CPU_CNT-0001-0002-$configIndex.conf
      sed -i\
         -e "${insertPoint}i$processorSkew" \
         $newFilename
      let index--
   done
   if [[ $DOS2UNIX != '' ]]
   then
      $DOS2UNIX $newFilename
   fi
done
