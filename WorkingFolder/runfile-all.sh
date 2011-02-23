#!/bin/bash
HOSTNAME=$(hostname)

./runfile-barnes-origin-all.sh
./runfile-barnes-directory-all.sh
./runfile-cholesky-origin-all.sh
./runfile-cholesky-directory-all.sh
./runfile-fft-origin-all.sh
./runfile-fft-directory-all.sh
./runfile-fmm-origin-all.sh
./runfile-fmm-directory-all.sh
./runfile-radix-origin-all.sh
./runfile-radix-directory-all.sh
./runfile-raytrace-origin-all.sh
./runfile-raytrace-directory-all.sh
./runfile-ocean-origin-all.sh
./runfile-ocean-directory-all.sh
