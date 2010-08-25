#/bin/bash
for file in $(ls genome__64_1_8_2_Perfect.conf_??); do
   cp $file "genome-02cpu-${file:30:2}".conf
done
