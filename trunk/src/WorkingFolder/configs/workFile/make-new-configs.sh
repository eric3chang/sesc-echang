#/bin/bash
# create more config files from old ones

#set -x   # verbose output
OLD_BENCHMARK=fft  # benchmark name of old config files
NEW_BENCHMARK=fft  # benchmark name of new config files
OLD_PROC_CNT=2   # number of processors in old config files
NEW_PROC_CNT=4   # number of processors in new config files

function appendZeros
{
   length=${#1}
   myString=$1
   if [[ length -lt 3 ]]
   then
      for i in {2..3}
      do
         myString=0${myString}
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
      -e "s@Directory_p$OLD_PROC_CNT@Directory_p$NEW_PROC_CNT@" \
      $file
done

