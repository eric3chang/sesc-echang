#/bin/bash
# renaming underscored files to hyphen files

for file in $(ls vacation_low__64_1_8_2_Perfect.conf_*); do
   string=${file:36:2}
   # if length of string is 1, append 0 in front of it
   if [[ ${#string} == 1 ]]
   then
      string=0$string
   fi
   cp $file "vacation-low-02cpu-$string".conf
done

