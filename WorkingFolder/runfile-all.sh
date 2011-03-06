#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)
AUGSESC=augSesc-Release

./runfile-radix-bip-all.sh
./runfile-radix-origin-all.sh
