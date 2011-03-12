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
cpuCountArray = ['002', '004', '008', '016', '032']
#cpuCountArray = ['032']
memdeviceArray = []

# determines suffix for input and output files
SUFFIX='.txt'

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
    formattedString = ''

    # process TotalRunTime
    for line in resultFile:
        if line.count(RESULTS_STRING):
            lineArray = line.split()
            isResult = 0
            for word in lineArray:
                if isResult:
                    outputFile.write(word)
                    formattedString += word+'\n'
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
                        formattedString += word
                    if word.count(tempMemDeviceString):
                        isResult = 1
                        outputFile.write(tempMemDeviceString+':')
    outputFile.write('\n')
    outputFile.write(formattedString)

    memdeviceFile.close()
    resultFile.close()
# end writeFinalResults

def make2Into1():
    # outputs the time + cache reads/misses into files
    for directoryType in directoryTypeArray:
        for cacheType in cacheTypeArray:
            for cpuCount in cpuCountArray:
                filenamePrefix = benchmarkName+'-'+directoryType+'-'+cacheType+'-'+cpuCount+'-0001-0002-00'
                resultFileString = RESULTS_FOLDER+filenamePrefix+SUFFIX
                memdeviceFileString = MEM_DEVICE_FOLDER+filenamePrefix+SUFFIX
                outputFileString = FINAL_RESULTS_FOLDER+benchmarkName+'-'+directoryType+'-'+cacheType+'-'+cpuCount+SUFFIX
                outputFile = open(outputFileString,'w')
                writeFinalResults(cpuCount, outputFile, resultFileString, memdeviceFileString)
                outputFile.close()

def makeTotalTimeFiles():
    # generate cpu file
    for cpuCount in cpuCountArray:
        totalRuntimeString = ''
        totalRuntimeStringFormatted = ''
        for directoryType in directoryTypeArray:
            for cacheType in cacheTypeArray:
                infilename = FINAL_RESULTS_FOLDER+benchmarkName+'-'+directoryType+'-'+cacheType+'-'+cpuCount+SUFFIX
                infile = open(infilename,'r')
                for line in infile:
                    if line.count(RESULTS_STRING):
                        lineSplit = line.split(':')
                        totalRuntimeString += lineSplit[1]
                        totalRuntimeStringFormatted += line
                infile.close()

        outfilename = FINAL_RESULTS_FOLDER+benchmarkName+'-'+cpuCount+'-'+RESULTS_STRING+SUFFIX
        outfile = open(outfilename,'w')
        outfile.write(totalRuntimeStringFormatted)
        outfile.write('\n')
        outfile.write(totalRuntimeString)
        outfile.close()

def makeGraphInput():
    for cpuCount in cpuCountArray:
        outFilename = benchmarkName+'-'+cpuCount+'-graph-input'+SUFFIX
        outFile = open(FINAL_RESULTS_FOLDER+outFilename,'w')
        for directoryType in directoryTypeArray:
            for cacheType in cacheTypeArray:
                if directoryType=='dir':
                    newDirectoryType = 'ogdir'
                else:
                    newDirectoryType = 'sodir'
                outFile.write(benchmarkName+' '+newDirectoryType+' '+cacheType)
                inFilename = benchmarkName+'-'+directoryType+'-'+cacheType+'-'+cpuCount+SUFFIX
                inFile = open(FINAL_RESULTS_FOLDER+inFilename,'r')
                for line in inFile:
                    if line.count(RESULTS_STRING):
                        lineSplit = line.strip().split(':')
                        outFile.write(','+lineSplit[1])
                    for cacheStatistic in memdeviceArray:
                        if line.count(cacheStatistic):
                            lineSplit = line.strip().split(':')
                            outFile.write(','+lineSplit[1])
                outFile.write('\n')
                inFile.close()
        outFile.close()

make2Into1()
makeTotalTimeFiles()
makeGraphInput()



