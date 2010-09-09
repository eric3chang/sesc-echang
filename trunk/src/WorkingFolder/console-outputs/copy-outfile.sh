#!/bin/bash
hostname=$(hostname)

cp $1 $1.$hostname
if [[ $2 != '' ]]
then
   cat $1.$hostname | grep $2 > $2
fi

