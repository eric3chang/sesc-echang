#/bin/bash
# rename files to create more config files

#set -x   # verbose output

OLD_FILENAME=fft-002-0001-0002-??.conf
NEW_FILENAME_PREFIX=fft-3sd-moesi-002-0001-0002

for file in $(ls $OLD_FILENAME); do
   string=${file:0-7:2}
   cp $file "${NEW_FILENAME_PREFIX}-${string}".conf
done


