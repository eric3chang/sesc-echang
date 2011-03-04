#!/bin/bash
DATE=$(date "+%m%d%H%M")
HOSTNAME=$(hostname)

./runfile-cholesky-bip-all.sh
./runfile-cholesky-origin-all.sh
