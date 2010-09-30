#!/bin/bash

if [[ $1 == '' ]]
then
   echo 'no argument'
else
   cat $1 | sed 's/msg=[ ]*/msg=/' | tr ' ' '\n' | grep msg= | cut -d '=' -f 2 | uniq -c | sort -n
fi
