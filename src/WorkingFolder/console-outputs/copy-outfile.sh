#!/bin/bash
set -x
hostname=$(hostname)

if [[ $1 == '' ]]
then
   echo 'not enough arguments'
   exit
fi

# if the result doesn't already exist and we are not using the result itself
if [[ !(-e $1.$hostname) && !($1 =~ $hostname) ]]
then
   cp $1 $1.$hostname
fi

# grep for $2. $2 could be messageID or addr
if [[ $2 != '' ]]
then
   cat $1.$hostname | grep -w "$2" > "$2.temp"
fi

