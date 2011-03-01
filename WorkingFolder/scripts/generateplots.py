#!/usr/bin/python
import os
import sys

# need to change this when moving this script
IN_DIR='../results/'
OUT_DIR='./graphs/'
#OUT_DIR='../configs/workFile/'

# don't need to change these when moving this script
IN_EXT='.memDevResults'
OUT_EXT='.png'

thisFileName = sys.argv[0]
thisFileName = os.path.basename(thisFileName)

def printError():
    print ("usage: " + 'GenerateSingleConfig' +
        " benchmarkName directoryType processorCount L1Size L2Size")
    quit()

# converts the parameter to int with error checking
def convertToInt(incoming):
    try:
        outgoing = int(incoming)
    except ValueError:
        print(incoming + " is not an integer")
        quit()
    return outgoing

def getDictionary(benchmark, dirtype, cpu, l1, l2):
    fullpath = getFilename(benchmark, dirtype, cpu, l1, l2)
    filein = open(fullpath, 'r')
    dictionary = []
    line = ''
    for line in filein:
        line = line.strip()
        if (line.count(':')):
            splitlines = line.split(':')
            key = splitlines[0]
            value = splitlines[1]
            keyvalue = [key, value]
            dictionary.append(keyvalue)
    return dictionary

def getFilename(benchmark, dirtype, cpu, l1, l2):
    filename = benchmark+'-'+dirtype+'-'+cpu+'-'+l1+'-'+l2+IN_EXT
    fullpath = IN_DIR+filename
    return fullpath

def main():
    #benchmarkNames = ['barnes', 'cholesky', 'fft', 'fmm', 'radix', 'raytrace', 'ocean']
    benchmarkNames = ['fft']
    #directoryTypes = ['bip', 'directory', 'origin']
    directoryTypes = ['bip']
    #cacheType = 'mesi'
    #cacheType = ''
    processorCountLow = '2'
    processorCountHi = '32'
    L1Low = '8'
    L1Hi = '1024'

    dictionary = getDictionary('fft', 'bip', '2', '8', '16')
    print (dictionary)

    #for directory in directoryTypes:
       #generateAllBenchmarks(benchmarkNames, directory, processorCountLow, processorCountHi, L1Low, L1Hi)

if __name__ == "__main__":
    main()
