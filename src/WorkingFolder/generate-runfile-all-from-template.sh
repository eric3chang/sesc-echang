#!/bin/bash
#set -x   # enable debug messages

RUNFILE_PREFIX=runfile-
RUNFILE_SUFFIX=-all.sh
TEMPLATE_NAME=runfile-all-template

function appendZeros
{
   length=${#1}
   myString=$1
   if [[ length -lt 3 ]]
   then
      index=$length
      while [[ $index -lt 3 ]]
      do
         myString=0${myString}
         let index++
      done
   fi
   echo $myString
}

#while [[ $1 != '' ]]; do
#   echo $1
#   shift
#done

if [[ $3 == '' ]]; then
   echo 'usage: generate-runfile-all-from-template.sh BENCHMARK_NAME PARAMETER_PREFIX PARAMETER_SUFFIX'
   exit 1
fi

BENCHMARK_NAME=$1
PARAMETER_PREFIX=$2
PARAMETER_SUFFIX=$3

# generate a new runfile-all file based on the template
newfilename=${RUNFILE_PREFIX}${BENCHMARK_NAME}${RUNFILE_SUFFIX}
cp $TEMPLATE_NAME $newfilename

# replace BENCHMARK_NAME with desired benchmark name
sed -i "s/BENCHMARK_NAME/$BENCHMARK_NAME/g" $newfilename

# replace CPUXXX_PARAMETERS with new parameters
cpuArray=(2 4 8 16 32)
for cpu in ${cpuArray[*]}; do
   appendedCpu=$(appendZeros $cpu)
   oldParameters=CPU${appendedCpu}_PARAMETERS
   newParameters="${PARAMETER_PREFIX}${cpu}${PARAMETER_SUFFIX}"
   sed -i "s@$oldParameters@$newParameters@g" $newfilename
done

