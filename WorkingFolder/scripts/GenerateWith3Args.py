#!/usr/bin/python
import sys

import GenerateMultipleConfigs

def printError():
    print ("usage: " + 'GenerateWith3Args' +
        " benchmarkName directoryType cacheType")
    quit()

def main():
    if (len(sys.argv) != 4):
        printError()

    benchmarkName = sys.argv[1]
    directoryType = sys.argv[2]
    cacheType = sys.argv[3]
    processorCountLow = str(2)
    processorCountHi = str(64)
    L1Low = str(1)
    L1Hi = str(1024)

    GenerateMultipleConfigs.generateConfigs(benchmarkName, directoryType, cacheType, processorCountLow, processorCountHi, L1Low, L1Hi)

if __name__ == "__main__":
    main()