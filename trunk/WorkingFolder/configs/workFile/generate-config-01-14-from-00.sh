#!/bin/bash
# generates config files 01-14 from config file 00
#set -x  # turns debug mode on

OLD_FILENAME="fft-3sd-moesi-002-0001-0002-00.conf"

filenameLength=${#OLD_FILENAME}
let shortFilenameLength=$filenameLength-7
newFilenamePrefix=${OLD_FILENAME:0:$shortFilenameLength}

for i in {1..14}
do
   if [[ ${#i} == 1 ]]
   then
      i=0$i
   fi
   cp $OLD_FILENAME $newFilenamePrefix$i.conf
done

