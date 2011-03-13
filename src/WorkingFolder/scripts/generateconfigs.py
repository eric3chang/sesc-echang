#!/users/erichang/csl/opt/bin/python
import os
import sys

# need to change this when moving this script
#OUTPUT_DIRECTORY='./configs/'
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

def generateConfig(benchmarkName, directoryType, processorCount, L1Size, L2Size, memoryfilePrefix):
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
    # don't need to write ReportFile, since it has no useful information
    #outFile.write("ReportFile = 'results/" + outFilename + ".report'\n")
    outFile.write("MemDeviceReportFile = 'results/" + outFilename + ".memDevResults'\n")
    outFile.write("CompositionResultFile = 'results/" + outFilename + ".dat'\n")
    outFile.write("BenchName = '" + benchmarkName + "'\n")
    outFile.write('MemorySystemConfig = "' + MEMORY_SYSTEM_CONFIG_DIRECTORY + memoryfilePrefix + directoryType \
        + '-p' + processorCount + '-c' + L1Size + 'L1-' + L2Size + 'L2.memory"\n')
    outFile.write('<base_' + processorCount + 'cpu.conf>\n')
    outFile.write('\n')

    outFile.close()

def generateMultipleConfigs(benchmarkName, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi,
    memoryfilePrefix,L2Low,L2Hi):
    # check if these variables are numbers before using them
    processorCountLowInt = convertToInt(processorCountLow)
    processorCountHiInt = convertToInt(processorCountHi)
    L1LowInt = convertToInt(L1Low)
    L1HiInt = convertToInt(L1Hi)
    L2LowInt = convertToInt(L2Low)
    L2HiInt = convertToInt(L2Hi)

    processorIndex = processorCountLowInt
    while (processorIndex <= processorCountHiInt):
        L1Index = L1LowInt
        while (L1Index <= L1HiInt):
            L2Index = L2LowInt
            while (L2Index <= L2HiInt):
                generateConfig(benchmarkName, directoryType, str(processorIndex), str(L1Index), \
                    str(L2Index),memoryfilePrefix)
                #print(str(processorIndex) + ' ' + str(L1Index) + ' ' + str(L2Index))
                L2Index *= 2
            L1Index *= 2
        processorIndex *=2

def generateAllBenchmarks(benchmarkNames, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi,
    memoryfilePrefix,L2Low,L2Hi):
    for benchmark in benchmarkNames:
        generateMultipleConfigs(benchmark, directoryType, processorCountLow, processorCountHi, L1Low, L1Hi,
            memoryfilePrefix,L2Low,L2Hi)

def main():
    #benchmarkNames = ['cholesky', 'fft', 'lu', 'newtest', 'radix', 'raytrace', 'ocean']
    benchmarkNames = ['newtest']
    directoryTypes = ['bip','origin']
    processorCountLow = '4'
    processorCountHi = '32'
    L1Low = '64'
    L1Hi = '64'
    L2Low = '128'
    L2Hi = '4096'
    memoryfilePrefix = ''
    #memoryfilePrefix = 'network10-'
    #memoryfilePrefix = 'network0.3-'

    for directory in directoryTypes:
       generateAllBenchmarks(benchmarkNames, directory, processorCountLow, processorCountHi, L1Low, L1Hi,memoryfilePrefix,L2Low,L2Hi)

if __name__ == "__main__":
    main()
