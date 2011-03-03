#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

./runfile-lu-bip-all.sh
./runfile-lu-origin-all.sh
