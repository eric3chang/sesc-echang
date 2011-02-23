#!/bin/bash
HOSTNAME=$(hostname)

./runfile-barnes-origin-all.sh
./runfile-barnes-directory-all.sh
./runfile-cholesky-origin-all.sh
./runfile-cholesky-directory-all.sh
