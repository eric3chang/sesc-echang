#!/usr/bin/python
import sys

import GenerateMultipleConfigs

def printError():
    print ("usage: " + 'GenerateAllBenchmarks' +
        " benchmark ...")
    quit()

# converts the parameter to int with error checking
def convertToInt(incoming):
    try:
        outgoing = int(incoming)
    except ValueError:
        print(incoming + " is not an integer")
        quit()
    return outgoing

def generateAllBenchmarks(benchmarkNames, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi):
    for benchmark in benchmarkNames:
        GenerateMultipleConfigs.generateConfigs(benchmark, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi)

def main():
    benchmarkNames = ['barnes', 'cholesky', 'fft', 'fmm', 'radix', 'raytrace', 'ocean']
    #directoryType = 'origin'
    #directoryType = 'directory'
    directoryType = 'bip'
    #cacheType = 'mesi'
    #cacheType = ''
    processorCountLow = '2'
    processorCountHi = '32'
    L1Low = '8'
    L1Hi = '1024'

    generateAllBenchmarks(benchmarkNames, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi)

if __name__ == "__main__":
    main()