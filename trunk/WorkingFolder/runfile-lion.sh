#!/bin/bash
HOSTNAME=$(hostname)

./runfile-raytrace-origin-all.sh
./runfile-raytrace-bip-all.sh
./runfile-ocean-origin-all.sh
./runfile-ocean-bip-all.sh
