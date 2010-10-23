#/bin/bash
# rename files to create more config files

#set -x   # verbose output

OLD_PREFIX=barnes
NEW_PREFIX=barnes-3sd-moesi

OLD_FILENAME=$OLD_PREFIX-002-0001-0002-??.conf
NEW_FILENAME_PREFIX=$NEW_PREFIX-002-0001-0002

for file in $(ls $OLD_FILENAME); do
   # gets the string of 00-14
   string=${file:0-7:2}
   newFilename=${NEW_FILENAME_PREFIX}-${string}
   cp $file $newFilename.conf
   sed -i 's/Directory/3StageDirectoryMOESI/' $newFilename.conf
   sed -i "9iMemDeviceReportFile = 'mem-device-results/$newFilename.txt'" $newFilename.conf
done

