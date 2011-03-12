#!/bin/bash
# replace processor skews in config files with new ones

OLD_FILENAME_PREFIX="fft-002-0001-0002-"
NEW_FILENAME_PREFIX="fft-3sd-moesi-002-0001-0002-"
PROCESSOR_COUNT=2

for i in {1..14}
do
   if [[ ${#i} == 1 ]]
   then
      i=0$i
   fi
   oldFilename=$OLD_FILENAME_PREFIX$i.conf
   newFilename=$NEW_FILENAME_PREFIX$i.conf
   sed -i\
      "/ProcessorSkew/ d" \
      $newFilename

   let index=$PROCESSOR_COUNT-1
   while [[ $index -ge 0 ]]
   do
      processorSkew="ProcessorSkew\[$index]"
      processorSkew2=$(cat $oldFilename | grep "$processorSkew" )
      sed -i\
         "9i$processorSkew2" \
         $newFilename
      let index--
   done
done

