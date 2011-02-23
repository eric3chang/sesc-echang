#!/bin/bash
HOSTNAME=$(hostname)

./runfile-fft-origin-all.sh
./runfile-fft-bip-all.sh
#./runfile-fmm-origin-all.sh
#./runfile-fmm-bip-all.sh
./runfile-radix-origin-all.sh
./runfile-radix-bip-all.sh
