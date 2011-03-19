#!/bin/bash

if [[ $1 == '' ]]
then
   echo 'no argument'
else
   cat $1 | sed 's/msg=[ ]*/msg=/' | tr ' ' '\n' | grep 'msg=' | cut -d '=' -f 2 | sort -n | uniq -c | sort -n > $1.msgCounts
fi
