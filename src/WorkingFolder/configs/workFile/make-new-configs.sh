#/bin/bash
# create more config files from old ones

#set -x   # verbose output
OLD_BENCHMARK=fft-3sd-moesi  # benchmark name of old config files
NEW_BENCHMARK=fft-3sd-moesi  # benchmark name of new config files
OLD_PROC_CNT=4   # number of processors in old config files
NEW_PROC_CNT=8   # number of processors in new config files

function appendZeros
{
   length=${#1}
   myString=$1
   if [[ length -lt 3 ]]
   then
      index=$length
      while [[ $index -lt 3 ]]
      do
         myString=0${myString}
         let index++
      done
   fi
   echo $myString
}

OLD_PROC_CNT_APP=$(appendZeros $OLD_PROC_CNT)
NEW_PROC_CNT_APP=$(appendZeros $NEW_PROC_CNT)

# processor and cache settings of old config files
OLD_SETTING=$OLD_PROC_CNT_APP-0001-0002
# processor and cache settings of new config files
NEW_SETTING=$NEW_PROC_CNT_APP-0001-0002

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

for file in $(ls $OLD_BENCHMARK-$OLD_SETTING-??.conf); do
   string=${file:0-7:2}
   cp $file "$NEW_BENCHMARK-$NEW_SETTING-$string".conf
done

for file in $(ls $NEW_BENCHMARK-$NEW_SETTING-??.conf); do
   truncatedFile=${file%.*}
#      -e 's/134217728/334217728/' \
#      -e 's/CheckpointName/#CheckpointName/' \
#      -e 's/ReportFile/#ReportFile/' \
#      -e "s@results/genome-02cpu@results/$NEW_BENCHMARK@" \
#      -e "s/BenchName = 'fft'/BenchName = 'cholesky'/" \
   sed -i\
      -e "s@$OLD_BENCHMARK@$NEW_BENCHMARK@" \
      -e "s/-$OLD_PROC_CNT_APP/-$NEW_PROC_CNT_APP/" \
      -e "s/_$OLD_PROC_CNT_APP/_$NEW_PROC_CNT_APP/" \
      -e "s@_p$OLD_PROC_CNT@_p$NEW_PROC_CNT@" \
      $file
   if [[ $DOS2UNIX != '' ]]
   then
      $DOS2UNIX $file
   fi
done

