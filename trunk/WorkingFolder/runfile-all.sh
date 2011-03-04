#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

./runfile-fft-bip-all.sh
./runfile-fft-origin-all.sh
