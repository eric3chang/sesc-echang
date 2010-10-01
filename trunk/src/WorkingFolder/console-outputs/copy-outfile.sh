#!/bin/bash
set -x
hostname=$(hostname)

if [[ $1 == '' ]]
then
   echo 'not enough arguments'
   exit
fi

# if the result doesn't already exist and we are not using the result itself
#if [[ !(-e $1.$hostname) && !($1 =~ $hostname) ]]
#then
   cp $1 $1.$hostname
#fi

