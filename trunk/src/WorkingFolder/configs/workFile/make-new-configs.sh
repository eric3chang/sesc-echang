#/bin/bash
# create more config files from old ones

#set -x   # verbose output
OLD_BENCHMARK=cholesky # benchmark name of old config files
OLD_SETTING=002-0001-0002  # processor and cache settings of old config files
NEW_BENCHMARK=fft   # benchmark name of new config files
NEW_SETTING=002-0001-0002  # processor and cache settings of new config files

for file in $(ls $OLD_BENCHMARK-$OLD_SETTING-??.conf); do
   string=${file:0-7:2}
   cp $file "$NEW_BENCHMARK-$NEW_SETTING-$string".conf
done

for file in $(ls $NEW_BENCHMARK-$NEW_SETTING-??.conf); do
   truncatedFile=${file%.*}
#      -e 's/134217728/334217728/' \
#      -e 's/CheckpointName/#CheckpointName/' \
#      -e 's/ReportFile/#ReportFile/' \
#      -e "s/chk_genome_2.checkpoint/cholesky-002.checkpoint/" \
#      -e "s@results/genome-02cpu@results/$NEW_BENCHMARK@" \
#      -e "s/BenchName = 'fft'/BenchName = 'cholesky'/" \
   sed -i\
      -e "s@$OLD_BENCHMARK@$NEW_BENCHMARK@" \
      $file
done

