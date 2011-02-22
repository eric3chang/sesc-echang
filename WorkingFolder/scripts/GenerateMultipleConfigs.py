#!/usr/bin/python
import os
import sys

import GenerateSingleConfig

thisFileName = sys.argv[0]
thisFileName = os.path.basename(thisFileName)

def printError():
    print ("usage: " + 'GenerateMultipleConfigs' +
        " benchmarkName directoryType processorCountLow processorCountHi L1Low L1Hi")
    quit()

# converts the parameter to int with error checking
def convertToInt(incoming):
    try:
        outgoing = int(incoming)
    except ValueError:
        print(incoming + " is not an integer")
        quit()
    return outgoing

def generateConfigs(benchmarkName, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi):
    # check if these variables are numbers before using them
    processorCountLowInt = convertToInt(processorCountLow)
    processorCountHiInt = convertToInt(processorCountHi)
    L1LowInt = convertToInt(L1Low)
    L1HiInt = convertToInt(L1Hi)

    processorIndex = processorCountLowInt
    while (processorIndex <= processorCountHiInt):
        L1Index = L1LowInt
        while (L1Index <= L1HiInt):
            L2Index = L1Index * 2
            while (L2Index <= L1HiInt*8):
                GenerateSingleConfig.generateConfig(benchmarkName, directoryType, \
                    str(processorIndex), str(L1Index), str(L2Index))
                #print(str(processorIndex) + ' ' + str(L1Index) + ' ' + str(L2Index))
                L2Index *= 2
            L1Index *= 2
        processorIndex *=2

def main():
    if (len(sys.argv) != 7):
        printError()

    benchmarkName = sys.argv[1]
    directoryType = sys.argv[2]
    processorCountLow = sys.argv[3]
    processorCountHi = sys.argv[4]
    L1Low = sys.argv[5]
    L1Hi = sys.argv[6]

    generateConfigs(benchmarkName, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi)

if __name__ == "__main__":
    main()