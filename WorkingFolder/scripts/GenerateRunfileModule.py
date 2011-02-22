#!/usr/bin/python
import os
import sys

# need to change this when moving this script
#OUT_DIR='./'
OUT_DIR='../'

# don't need to change these when moving this script
EXE_EXT='.mips'
OUT_EXT='.sh'
OUT_PRE='runfile-'
HEADER='#!/bin/bash\nHOSTNAME=$(hostname)\n\n'
STRING1='nice -10 ./augSesc-Release -cconfigs/workFile/'
STRING2='.conf -dconfigs/workFile/'
STRING3='.conf.report benchmarks-splash2-sesc/'
STRING4=' &> console-outputs/'
STRING5='.out.$HOSTNAME'

# parameters for benchmarks
FFT_PARAMS_PRE='-m10 -p'
FFT_PARAMS_POST=' -n65536 -l4 -t'

thisFileName = sys.argv[0]
thisFileName = os.path.basename(thisFileName)

# converts the parameter to int with error checking
def convertToInt(incoming):
    try:
        outgoing = int(incoming)
    except ValueError:
        print(incoming + " is not an integer")
        quit()
    return outgoing

def generateOneRunfile(benchmarkName, directoryType, processorCount, L1Size, L2Size):
    # check if these variables are numbers before using them
    processorCountInt = convertToInt(processorCount)
    L1SizeInt = convertToInt(L1Size)
    L2SizeInt = convertToInt(L2Size)

    if (benchmarkName=='fft'):
        #confFilename = FFT_PARAMS_PRE + processorCount + FFT_PARAMS_POST
        parameters = FFT_PARAMS_PRE + processorCount + FFT_PARAMS_POST
    else:
        print('Unknown benchmark ' + benchmarkName)
        quit()

    confFilename = benchmarkName + '-' + directoryType + '-' + processorCount + '-' + L1Size + '-' + L2Size

    returnString = STRING1 + confFilename + STRING2 + confFilename + STRING3 + benchmarkName + EXE_EXT\
        + ' ' + parameters + STRING4 + confFilename + STRING5
    return returnString

def generateMultipleRunfiles(benchmarkName, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi):
    # check if these variables are numbers before using them
    processorCountLowInt = convertToInt(processorCountLow)
    processorCountHiInt = convertToInt(processorCountHi)
    L1LowInt = convertToInt(L1Low)
    L1HiInt = convertToInt(L1Hi)

    outFilename = OUT_PRE + benchmarkName + '-' + directoryType + '-' \
        + processorCountLow + '-' + processorCountHi + '-' + L1Low + '-' + L1Hi
    outFilenameFull = outFilename + OUT_EXT

    outPath = OUT_DIR + outFilenameFull
    outFile = open(outPath, 'w')

    # start inputting data
    outputString = ''
    #outputString = generateOneRunfile(benchmarkName, directoryType, processorCountLow, L1Low, '4096')
    outFile.write(HEADER)

    processorIndex = processorCountLowInt
    while (processorIndex <= processorCountHiInt):
        L1Index = L1LowInt
        while (L1Index <= L1HiInt):
            L2Index = L1Index * 2
            while (L2Index <= L1HiInt*8):
                outputString += generateOneRunfile(benchmarkName, directoryType, str(processorIndex), str(L1Index), str(L2Index))
                outputString += '\n'
                #print(str(processorIndex) + ' ' + str(L1Index) + ' ' + str(L2Index))
                L2Index *= 2
            L1Index *= 2
        processorIndex *=2

    outFile.write(outputString)
    outFile.close()

def main():
    #benchmarkNames = ['barnes', 'cholesky', 'fft', 'fmm', 'radix', 'raytrace', 'ocean']
    benchmarkNames = ['fft']
    directoryType = 'origin'
    #directoryType = 'directory'
    #directoryType = 'bip'
    #cacheType = 'mesi'
    #cacheType = ''
    processorCountLow = '2'
    processorCountHi = '32'
    L1Low = '8'
    L1Hi = '1024'

    for name in benchmarkNames:
        generateMultipleRunfiles(name, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi)

if __name__ == "__main__":
    main()