#!/bin/bash
HOSTNAME=$(hostname)

./runfile-fft-origin-all.sh
./runfile-fft-directory-all.sh
./runfile-fmm-origin-all.sh
./runfile-fmm-directory-all.sh
./runfile-radix-origin-all.sh
./runfile-radix-directory-all.sh
