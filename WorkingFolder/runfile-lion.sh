#!/bin/bash
HOSTNAME=$(hostname)

./runfile-raytrace-origin-all.sh
./runfile-raytrace-directory-all.sh
./runfile-ocean-origin-all.sh
./runfile-ocean-directory-all.sh
