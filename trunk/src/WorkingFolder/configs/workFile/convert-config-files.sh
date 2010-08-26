#/bin/bash
# transform old conf files into new conf files

#file='vacation-high-02cpu-00.conf'
for file in $(ls yada-dots-02cpu-??.conf); do
   truncatedFile=${file%.*}

   # add commented 1st line for auto syntax highlighting
   sed -i -e '1i\#' \
      -e 's/chk_yada_2.checkpoint/yada-dots-02cpu.checkpoint/' \
      -e 's/DieAfterCheckpointTaken = 1/DieAfterCheckpointTaken = 0/' \
      -e "s_results\\\.*.txt_results/$truncatedFile.txt_" \
      -e 's@<base.conf>@MemorySystemConfig = "memgen/memoryConfigs/Directory_p2_c1L1-2L2.memory"@' \
      -e 's@<memSysConfigs/MemConfig_1_8_2.conf>@<base_02cpu.conf>@' \
      $file

   dos2unix $file
done

