#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

./runfile-barnes-bip-all.sh
./runfile-barnes-origin-all.sh
