#!/usr/bin/python
import os
import sys

# need to change this when moving this script
#OUTPUT_DIRECTORY='./'
OUTPUT_DIRECTORY='../configs/workFile/'

# don't need to change these when moving this script
MEMORY_SYSTEM_CONFIG_DIRECTORY='memgen/memoryConfigs/'
OUTPUT_EXTENSION='.conf'

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

def generateConfig(benchmarkName, directoryType, processorCount, L1Size, L2Size):
    # check if these variables are numbers before using them
    processorCountInt = convertToInt(processorCount)
    L1SizeInt = convertToInt(L1Size)
    L2SizeInt = convertToInt(L2Size)

    outFilename = benchmarkName + '-' + directoryType + '-' \
        + processorCount + '-' + L1Size + '-' + L2Size
    outFilenameFull = outFilename + OUTPUT_EXTENSION

    outPath = OUTPUT_DIRECTORY + outFilenameFull
    outFile = open(outPath, 'w')

    # start inputting data
    outFile.write('ShouldLoadCheckpoint = 0\n')
    outFile.write('CheckpointName = "checkpoints\\' + outFilename + '.checkpoint"\n')
    outFile.write('DieAfterCheckpointTaken = 0\n')
    outFile.write('HeapMemorySize = 334217728\n')
    outFile.write('FilterSize = 64\n')
    outFile.write("ReportFile = 'results/" + outFilename + ".report'\n")
    outFile.write("MemDeviceReportFile = 'results/" + outFilename + ".memDevResults'\n")
    outFile.write("CompositionResultFile = 'results/" + outFilename + ".dat'\n")
    outFile.write("BenchName = '" + benchmarkName + "'\n")
    outFile.write('MemorySystemConfig = "' + MEMORY_SYSTEM_CONFIG_DIRECTORY + directoryType \
        + '-p' + processorCount + '-c' + L1Size + 'L1-' + L2Size + 'L2.memory"\n')
    outFile.write('<base_' + processorCount + 'cpu.conf>\n')
    outFile.write('\n')

    outFile.close()

def main():
    if (len(sys.argv) != 6):
        printError()

    benchmarkName = sys.argv[1]
    directoryType = sys.argv[2]
    processorCount = sys.argv[3]
    L1Size = sys.argv[4]
    L2Size = sys.argv[5]

    generateConfig(benchmarkName, directoryType, processorCount, L1Size, L2Size)

if __name__ == "__main__":
    main()