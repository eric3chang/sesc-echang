#/bin/bash
# transform old conf files into splash conf files

for file in $(ls cholesky-02cpu-??.conf); do
   truncatedFile=${file%.*}
   sed -i\
      -e 's/ShouldLoadCheckpoint = 1/ShouldLoadCheckpoint = 0/' \
      -e 's/CheckpointName/#CheckpointName/' \
      -e 's/134217728/334217728/' \
      -e 's/ReportFile/#ReportFile/' \
      -e "s/BenchName = 'genome'/BenchName = 'fft'/" \
      $file
done

