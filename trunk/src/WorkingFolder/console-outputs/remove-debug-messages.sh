#!/bin/bash

if [[ $# -lt 1 ]]
then
   echo "Usage remove-debug-messages.sh file"
   exit 1
fi

myFile=$1

sed \
   -e '/FastSimBeginEvent/ d' \
   -e '/has triggered a release/ d' \
   -e '/instructions so far at clock/ d' \
   -e '/Leaving rabbit mode.../ d' \
   -e '/Rabbit Mode Start/ d' \
   -e '/Releasing processor/ d' \
   -e '/^Stalling processor/ d' \
   -e '/^Tick/ d' \
   $myFile

