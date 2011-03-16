#!/bin/bash
IFS='
'

FILES=$(ls -A)
counter=0
myString=""

for file in $FILES
do
    size=$(du -s $file)
   if [[ myString == "" ]]; then
       myString="$size"
   else
       myString="$myString"$'\n'"$size"
   fi
   counter=$[counter+1]
done

#echo "$myString"

size=$counter

echo "$myString" | sort -n

