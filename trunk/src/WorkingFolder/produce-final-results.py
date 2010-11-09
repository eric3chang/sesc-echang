#!/usr/bin/python
import sys

# these change the results that get put into the final result file
FINAL_RESULTS_FOLDER='final-results/'
MEM_DEVICE_FOLDER='mem-device-results/'
MEM_DEVICE_1=['totalL1Cache','totalL2Cache']
MEM_DEVICE_2=['Read','Write']
MEM_DEVICE_3=['Hits','Misses']
RESULTS_FOLDER='results/'
RESULTS_STRING='TotalRunTime'

# these change the input files that we are reading from
directoryTypeArray = ['dir', '3sd']
cacheTypeArray = ['moesi', 'msi']
#cpuCountArray = ['002', '004', '008', '016', '032']
cpuCountArray = ['032']
memdeviceArray = []

if (len(sys.argv) != 2):
    print('usage: produce-final-results.py BENCHMARK_NAME')
    exit()
else:
    benchmarkName = sys.argv[1]

# create totalL1CacheReadHits, totalL1CacheReadMisses,...
for word1 in MEM_DEVICE_1:
    for word2 in MEM_DEVICE_2:
        for word3 in MEM_DEVICE_3:
            memdeviceArray.append(word1+word2+word3)

def writeFinalResults(cpuCount, outputFile,resultFileString,memdeviceFileString):
    memdeviceFile = open(memdeviceFileString,'r')
    resultFile = open(resultFileString,'r')

    outputFile.write(benchmarkName+'-'+cpuCount+'\n')

    # process TotalRunTime
    for line in resultFile:
        if line.count(RESULTS_STRING):
            lineArray = line.split()
            isResult = 0
            for word in lineArray:
                if isResult:
                    outputFile.write(word)
                    isResult = 0
                if word.count(RESULTS_STRING):
                    outputFile.write(RESULTS_STRING+':')
                    isResult = 1
    outputFile.write('\n')
    # process cache hits and misses
    for line in memdeviceFile:
        for tempMemDeviceString in memdeviceArray:
            if line.count(tempMemDeviceString):
                splitLine = line.split(':')
                isResult = 0
                for word in splitLine:
                    if isResult:
                        isResult = 0
                        outputFile.write(word)
                    if word.count(tempMemDeviceString):
                        isResult = 1
                        outputFile.write(tempMemDeviceString+':')

    memdeviceFile.close()
    resultFile.close()


'''
# debug code
outputFileString = FINAL_RESULTS_FOLDER+benchmarkName+'.txt'
outputFile = open(outputFileString,'w')
debugFileString = 'fft-002-0001-0002-00.txt'
resultFile = RESULTS_FOLDER+debugFileString
memdeviceFile = MEM_DEVICE_FOLDER+debugFileString
writeFinalResults('002',outputFile, resultFile, memdeviceFile)
outputFile.close()
# end debug code
'''


for directoryType in directoryTypeArray:
    for cacheType in cacheTypeArray:
        for cpuCount in cpuCountArray:
            filenamePrefix = benchmarkName+'-'+directoryType+'-'+cacheType+'-'+cpuCount+'-0001-0002-00'
            resultFileString = RESULTS_FOLDER+filenamePrefix+'.txt'
            memdeviceFileString = MEM_DEVICE_FOLDER+filenamePrefix+'.txt'
            outputFileString = FINAL_RESULTS_FOLDER+benchmarkName+'-'+cpuCount+'.txt'
            outputFile = open(outputFileString,'w')
            writeFinalResults(cpuCount, outputFile, resultFileString, memdeviceFileString)
            outputFile.close()
