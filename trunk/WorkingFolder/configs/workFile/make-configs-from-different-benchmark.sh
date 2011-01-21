#/bin/bash
# rename files to create more config files
#set -x   # verbose output

if [[ $1 == '' ]]; then
   echo "usage: make-configs-from-different-benchmark NEW_BENCHMARK"
   exit 1
fi

OLD_BENCHMARK=fft
NEW_BENCHMARK=$1

L1_SIZE=0001
L2_SIZE=0002

DIR_TYPES=(dir 3sd)
CACHE_TYPES=(moesi msi)
CPU_COUNTS=(002 004 008 016 032)

for dirType in ${DIR_TYPES[*]}; do
   for cacheType in ${CACHE_TYPES[*]}; do
      for cpuCount in ${CPU_COUNTS[*]}; do
         suffix="-$dirType-$cacheType-$cpuCount-$L1_SIZE-$L2_SIZE-00.conf"
         oldname=${OLD_BENCHMARK}${suffix}
         newname=${NEW_BENCHMARK}${suffix}
         cp $oldname $newname
         sed -i "s/$OLD_BENCHMARK/$NEW_BENCHMARK/g" $newname
      done
   done
done

