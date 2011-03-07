#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

#./runfile-barnes-bip-all.sh
#./runfile-barnes-origin-all.sh
#./runfile-cholesky-bip-all.sh
#./runfile-cholesky-origin-all.sh
#./runfile-fft-bip-all.sh
#./runfile-fft-origin-all.sh
./runfile-lu-bip-all.sh
./runfile-lu-origin-all.sh
#./runfile-newtest-bip-all.sh
#./runfile-newtest-origin-all.sh
#./runfile-radix-bip-all.sh
#./runfile-radix-origin-all.sh
./runfile-raytrace-bip-all.sh
./runfile-raytrace-origin-all.sh
./runfile-ocean-bip-all.sh
./runfile-ocean-origin-all.sh
