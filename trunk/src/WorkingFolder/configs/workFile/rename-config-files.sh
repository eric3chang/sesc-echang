#/bin/bash
# renaming underscored files to hyphen files

for file in $(ls vacation_high__64_1_8_2_Perfect.conf_*); do
   string=${file:0-2}
   # if length of string is 1, append 0 in front of it
   if [[ ${string:0:1} == _ ]]
   then
      string=0${string:1:1}
   fi
   cp $file "vacation-high-02cpu-$string".conf
done

