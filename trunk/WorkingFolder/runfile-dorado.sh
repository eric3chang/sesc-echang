#!/bin/bash
HOSTNAME=$(hostname)

./runfile-barnes-origin-all.sh
./runfile-barnes-bip-all.sh
./runfile-cholesky-origin-all.sh
./runfile-cholesky-bip-all.sh
