#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Debug

./runfile-newtest-bip-all.sh
./runfile-newtest-origin-all.sh
