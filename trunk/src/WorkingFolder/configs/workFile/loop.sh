#/bin/bash
# renaming underscored files to hyphen files
#for file in $(ls genome__64_1_8_2_Perfect.conf_??); do
#   cp $file "genome-02cpu-${file:30:2}".conf
#done

# transform old conf files into new conf files
#file='genome-02cpu-01.conf'
for file in $(ls genome-02cpu-??.conf); do
   truncatedFile=${file%.*}
   sed -i -e '1i\#' \
      -e 's/DieAfterCheckpointTaken = 1/DieAfterCheckpointTaken = 0/' \
      -e "s_results\\\.*.txt_results/$truncatedFile.txt_" \
      -e 's/base.conf/base_02cpu.conf/' \
      -e 's@<memSysConfigs/MemConfig_1_8_2.conf>@@' \
      $file
done
      #-e "s@results\*.txt@results/$truncatedFile.txt@" \

