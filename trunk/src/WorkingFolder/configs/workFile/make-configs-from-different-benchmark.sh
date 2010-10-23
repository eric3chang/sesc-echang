#/bin/bash
# rename files to create more config files
#untested!

#set -x   # verbose output
OLD_BENCHMARK=fft
NEW_BENCHMARK=barnes

FILENAME_SUFFIX=-3sd-moesi-002-0001-0002-

OLD_FILENAME=${OLD_BENCHMARK}${FILENAME_SUFFIX}??.conf
NEW_FILENAME_PREFIX=${NEW_BENCHMARK}${FILENAME_SUFFIX}

for file in $(ls $OLD_FILENAME); do
   # gets the string of 00-14
   string=${file:0-7:2}
   newFilename=${NEW_FILENAME_PREFIX}${string}
   cp $file $newFilename.conf
   sed -i "s/$OLD_BENCHMARK/$NEW_BENCHMARK/" $newFilename.conf
done

