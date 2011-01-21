#!/bin/bash
# set -x #enable for debug outputs

if [[ $5 == '' ]]; then
   echo 'usage: copy-moesi-to-msi BENCHMARK_NAME DIRECTORY_TYPE CPU_COUNT L1_SIZE L2_SIZE'
   exit 1
fi

BENCHMARK=$1
DIRECTORY=$2
CPU=$3
L1_SIZE=$4
L2_SIZE=$5

#while [[ $1 != '' ]]; do
#   echo -n "$1 "
#   shift
#done

for file in $(ls "$BENCHMARK-$DIRECTORY-moesi-$CPU-$L1_SIZE-$L2_SIZE-"??".conf"); do
   currentConfig=${file:0-7:2}
   newfile=$BENCHMARK-$DIRECTORY-msi-$CPU-$L1_SIZE-$L2_SIZE-$currentConfig.conf
   cp $file $newfile
   sed -i 's/MOESI/MSI/' $newfile
   sed -i 's/moesi/msi/' $newfile
done

