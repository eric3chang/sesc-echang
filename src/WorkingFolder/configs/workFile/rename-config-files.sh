#/bin/bash
# rename files to create more config files

#for file in $(ls yada__64_1_8_2_Perfect.conf_*); do
   string=${file:0-2}
   # if length of string is 1, append 0 in front of it
   #if [[ ${string:0:1} == _ ]]
#   then
      string=0${string:1:1}
#   fi
#   cp $file "fft-02cpu-$string".conf
#done

for file in $(ls genome-02cpu-??.conf); do
   string=${file:0-7:2}
   cp $file "fft-02cpu-$string".conf
#   echo $file "fft-02cpu-$string".conf
done


