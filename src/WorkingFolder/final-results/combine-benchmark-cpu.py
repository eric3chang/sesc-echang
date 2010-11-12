#/usr/bin/python
import sys

SUFFIX='.txt'
USAGE_MESSAGE = 'Usage: combine-benchmark-cpu.py CPU_COUNT BENCHMARK1 BENCHMARK2 BENCHMARK3 ...'

# read in inputs
if len(sys.argv) < 3:
    print (USAGE_MESSAGE)
    exit(1)
cpuCount = sys.argv[1]
try:
    temp = int(cpuCount)
except ValueError:
    print (USAGE_MESSAGE)
    exit(1)

# open a new file to dump results into
outFilename = cpuCount+'-combined'+SUFFIX
outFile = open(outFilename,'w')

# read in files
for i in range(2,len(sys.argv)):
    benchmarkName = sys.argv[i]
    inFilename=benchmarkName+'-'+cpuCount+'-graph-input'+SUFFIX
    try:
        inFile = open(inFilename,'r')
    except IOError:
        print(inFilename+' does not exist')
        exit(1)
    for line in inFile:
        outFile.write(line)
    inFile.close()

outFile.close()
