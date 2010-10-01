#!/bin/bash
set -x

# grep for $2. $2 could be messageID or addr
if [[ $2 == '' ]]
then
   echo 'not enough arguments'
else
   cat $1 | grep -w "$2" > "$2.temp"
fi

