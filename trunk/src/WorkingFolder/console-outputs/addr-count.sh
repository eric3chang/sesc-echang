#!/bin/bash

if [[ $1 == '' ]]
then
   echo 'Usage: addr-count.sh OUT_FILE'
else
   sed 's/adr=[ ]*/adr=/' $1 | tr ' ' '\n' | grep 'adr=' | cut -d '=' -f 2 | sort -n | uniq -c | sort -n > $1.addrCounts
fi
