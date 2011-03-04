#!/users/erichang/csl/opt/bin/python
import os
import sys

from matplotlib.pyplot import *
#from mpl_toolkits.axes_grid1 import ImageGrid

# need to change this when moving this script
IN_DIR='../results/'
#OUT_DIR='./graphs/'
OUT_DIR='../../report/diagrams/'
#OUT_DIR='../configs/workFile/'

# don't need to change these when moving this script
DEFAULT_CPU='32'
#DEFAULT_L1='256'
DEFAULT_L1='128'
#DEFAULT_L2='4096'
DEFAULT_L2='1024'
GRAPH_WIDTH=2
IN_EXT='.memDevResults'
#LINESTYLES=['k--','k:']
LINESTYLES=['ko','k^']
#LINESTYLES=['k*','kD']
#LINESTYLES=['kp','kd']
LINESTYLE_SOLID='k-'
OUT_EXT='.png'

thisFileName = sys.argv[0]
thisFileName = os.path.basename(thisFileName)

def printError(errorMessage):
    print ('Error: ' + errorMessage)
    quit()

# converts the parameter to int with error checking
def convertToInt(incoming):
    try:
        outgoing = int(incoming)
    except ValueError:
        print(incoming + " is not an integer")
        quit()
    return outgoing

def getCpuResults(benchmarks, dirtypes, minCpu, maxCpu, l1, l2, component, key):
    benchmarkResults = {}
    mincpuInt = convertToInt(minCpu)
    maxcpuInt = convertToInt(maxCpu)
    for benchmark in benchmarks:
        dirtypeResults = {}
        for dirtype in dirtypes:
            cpuResults = {}
            cpu = mincpuInt
            while (cpu <= maxcpuInt):
                dictionary = getDictionary(benchmark, dirtype, str(cpu), l1, l2)
                if component not in dictionary:
                    printError(component + ' should be in the file')
                else:
                    subDictionary = dictionary[component]
                if key not in subDictionary:
                    printError(key + ' should be in the file')
                else:
                    value = subDictionary[key]
                    cpuResults[cpu] = value
                cpu *= 2
            dirtypeResults[dirtype] = cpuResults
        benchmarkResults[benchmark] = dirtypeResults
    return benchmarkResults

def getL1Results(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key):
    benchmarkResults = {}
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    for benchmark in benchmarks:
        dirtypeResults = {}
        for dirtype in dirtypes:
            iResults = {}
            i = minInt
            while (i <= maxInt):
                dictionary = getDictionary(benchmark, dirtype, cpu, str(i), l2)
                if component not in dictionary:
                    printError(component + ' should be in the file')
                else:
                    subDictionary = dictionary[component]
                if key not in subDictionary:
                    printError(key + ' should be in the file')
                else:
                    value = subDictionary[key]
                    iResults[i] = value
                i *= 2
            dirtypeResults[dirtype] = iResults
        benchmarkResults[benchmark] = dirtypeResults
    return benchmarkResults

def getDictionary(benchmark, dirtype, cpu, l1, l2):
    fullpath = getFilename(benchmark, dirtype, cpu, l1, l2)
    filein = open(fullpath, 'r')
    dictionary = {}
    line = ''
    for line in filein:
        line = line.strip()
        if (line.count(':')):
            splitlines = line.split(':')
            if (len(splitlines)!=3):
                printError('input file format should have dictionary of 3')
            component = splitlines[0]
            key = splitlines[1]
            value = splitlines[2]
            if component in dictionary:
                subDictionary = dictionary[component]
            else:
                subDictionary = {}
                dictionary[component] = subDictionary
            subDictionary[key] = value
    return dictionary

def getFilename(benchmark, dirtype, cpu, l1, l2):
    filename = benchmark+'-'+dirtype+'-'+cpu+'-'+l1+'-'+l2+IN_EXT
    fullpath = IN_DIR+filename
    return fullpath

def getGraphAverageResults(benchmarks, dirtypes, benchmarkResults, minimum, maximum):
    dirtypeResults = {}
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    maxValue = 0.0
    for dirtype in dirtypes:
        iResults = {}
        i = minInt
        while (i <= maxInt):
            total = 0
            for benchmark in benchmarks:
                dirtypeSources = benchmarkResults[benchmark]
                iSources = dirtypeSources[dirtype]
                value = iSources[i]
                total += float(value)
            average = total/len(benchmarks)
            iResults[i] = average
            if (average > maxValue):
                maxValue = average
            i *= 2
        dirtypeResults[dirtype] = iResults
    # normalize values
    for dirtype in dirtypes:
        i = minInt
        iSources = dirtypeResults[dirtype]
        while (i <= maxInt):
            value = iSources[i]
            value /= maxValue
            value *= 100
            iSources[i] = value
            i *= 2
    return dirtypeResults

def plotCpuLatencyMultiple(benchmarks, dirtypes, minimum, maximum):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, 'Network', 'AverageLatency', 'Average Latency (%)', 'cpu-latency')

def plotCpuLatencySingle(benchmarks, dirtypes, minimum, maximum):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum, 'Network', 'AverageLatency', 'Average Latency (%)', 'cpu-latency')

def plotCpuMessagesMultiple(benchmarks, dirtypes, minimum, maximum):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, 'Network', 'TotalMessagesReceived', 'Messages Received (%)', 'cpu-messages')

def plotCpuMessagesSingle(benchmarks, dirtypes, minimum, maximum):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum, 'Network', 'TotalMessagesReceived', 'Messages Received (%)', 'cpu-messages')

def plotCpuTimeMultiple(benchmarks, dirtypes, minimum, maximum):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, 'TotalRunTime', 'TotalRunTime', 'Runtime (%)', 'cpu-time')

def plotCpuTimeSingle(benchmarks, dirtypes, minimum, maximum):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum, 'TotalRunTime', 'TotalRunTime', 'Runtime (%)', 'cpu-time')

def plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, component, key, myYlabel, filenameSuffix):
    myXlabel='Number of Processors'
    iResults = getCpuResults(benchmarks, dirtypes, minimum, maximum, DEFAULT_L1, DEFAULT_L2, component, key)

    benchmarkLength = len(benchmarks)
    row = benchmarkLength / GRAPH_WIDTH
    row = int(row)
    remainder = benchmarkLength % GRAPH_WIDTH
    if (remainder > 0):
        row += 1
    figureIndexPrefix = str(row) + str(GRAPH_WIDTH)

    #myFigure = figure(1)
    #myGrid = ImageGrid(myFigure, 111, nrows_ncols = (row, GRAPH_WIDTH), axes_pad=0.1)
    subplots_adjust(hspace=0.4)
    subplots_adjust(wspace=0.4)

    figureIndex = 1
    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        graphResults = getGraphAverageResults(tempBenchmarks, dirtypes, iResults, minimum, maximum)
        subplotIn = figureIndexPrefix + str(figureIndex)
        subplot(subplotIn)
        plotGraph(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle)
        figureIndex += 1

    outfullpath = OUT_DIR+filenameSuffix
    savefig(outfullpath)
    #show()
    close()

def plotCpuSingle(benchmarks, dirtypes, minimum, maximum, component, key, myYlabel, filenameSuffix):
    myXlabel='Number of Processors'
    iResults = getCpuResults(benchmarks, dirtypes, minimum, maximum, DEFAULT_L1, DEFAULT_L2, component, key)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        graphResults = getGraphAverageResults(tempBenchmarks, dirtypes, iResults, minimum, maximum)
        plotGraph(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle)
        outfullpath = OUT_DIR+benchmark+'-'+filenameSuffix
        savefig(outfullpath)
        close()

def plotCpuAverage(benchmarks, dirtypes, mincpu, maxcpu, component, key):
    myXlabel='Number of Processors'
    myYlabel='Runtime (%)'
    cpuResults = getCpuResults(benchmarks, dirtypes, mincpu, maxcpu, DEFAULT_L1, DEFAULT_L2, component, key)
    myTitle = 'combined average'
    graphResults = getGraphAverageResults(benchmarks, dirtypes, cpuResults, mincpu, maxcpu)
    plotGraph(dirtypes, graphResults, mincpu, maxcpu, myXlabel, myYlabel, myTitle)

def plotGraph(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle):
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    linestyleIndex = 0
    linestyle = LINESTYLES[linestyleIndex]
    ticks = []
    if (myTitle!='radix'):
        for dirtype in dirtypes:
            iSources = graphResults[dirtype]
            i = minInt
            xValues = []
            yValues = []
            while (i <= maxInt):
                x = i
                y = iSources[i]
                xValues.append(x)
                yValues.append(y)
                ticks.append(i)
                i *= 2
            plot(xValues, yValues, linestyle, label=dirtype)
            plot(xValues, yValues, LINESTYLE_SOLID)
            # change line style
            linestyleIndex += 1
            if (linestyleIndex >= len(LINESTYLES)):
                linestyleIndex = 0
            linestyle = LINESTYLES[linestyleIndex]
    else:
        directoryIndex = len(dirtypes)-1
        while (directoryIndex >= 0):
            dirtype = dirtypes[directoryIndex]
            iSources = graphResults[dirtype]
            i = minInt
            xValues = []
            yValues = []
            while (i <= maxInt):
                x = i
                y = iSources[i]
                xValues.append(x)
                yValues.append(y)
                ticks.append(i)
                i *= 2
            specialDirtypeIndex = ~directoryIndex
            plot(xValues, yValues, linestyle, label=dirtypes[specialDirtypeIndex])
            plot(xValues, yValues, LINESTYLE_SOLID)
            # change line style
            linestyleIndex += 1
            if (linestyleIndex >= len(LINESTYLES)):
                linestyleIndex = 0
            linestyle = LINESTYLES[linestyleIndex]
            directoryIndex -= 1

    legend(loc='best')
    xlabel(myXlabel)
    ylabel(myYlabel)
    xticks(ticks)
    grid(True)
    title(myTitle)
    axis([0, maxInt*1.1, 0, 110])

def plotL1(benchmarks, dirtypes, minimum, maximum, component, key):
    myXlabel='L1 Cache Size in Kilobytes'
    myYlabel='% Runtime'
    results = getL1Results(benchmarks, dirtypes, DEFAULT_CPU, minimum, maximum, DEFAULT_L2, component, key)
    graphResults = getGraphAverageResults(benchmarks, dirtypes, results, minimum, maximum)
    plotGraph(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel)

def main():
    #benchmarks = ['barnes', 'cholesky', 'fft', 'fmm', 'radix', 'raytrace', 'ocean']
    #benchmarks = ['radix']
    benchmarks = ['fft', 'cholesky', 'ocean', 'radix']
    #dirtypes = ['bip', 'directory', 'origin']
    dirtypes = ['bip', 'origin']
    #cacheType = 'mesi'
    #cacheType = ''
    mincpu = '2'
    maxcpu = '32'
    minl1 = '16'
    maxl1 = '128'
    l1 = '128'
    l2 = '1024'
    componentKey = ['TotalRunTime', 'TotalRunTime']
    #componentKey = ['Network', 'AverageLatency']

    myXlabel='Number of Processors'
    myYlabel='% Runtime'

    #dictionary = getDictionary('combinedtest', 'bip', '4', '256', '4096')
    #dictionary = getDictionary('fft', 'bip', '2', '8', '16')
    #print (dictionary)
    '''
    cpuResults = getCpuResults(benchmarks, dirtypes, mincpu, maxcpu, l1, l2, component, key)
    graphResults = getGraphAverageResults(benchmarks, dirtypes, cpuResults, mincpu, maxcpu)
    plotGraph(dirtypes, graphResults, mincpu, maxcpu, myXlabel, myYlabel)
    print (graphResults)
    '''
    plotCpuTimeMultiple(benchmarks, dirtypes, mincpu, maxcpu)
    plotCpuMessagesMultiple(benchmarks, dirtypes, mincpu, maxcpu)
    plotCpuLatencyMultiple(benchmarks, dirtypes, mincpu, maxcpu)
    #plotCpuMessages(benchmarks, dirtypes, mincpu, maxcpu)
    #plotCpuLatency(benchmarks, dirtypes, mincpu, maxcpu)
    #plotL1(benchmarks, dirtypes, minl1, maxl1, componentKey[0], componentKey[1])
    #show()
    #for directory in directoryTypes:
       #generateAllBenchmarks(benchmarkNames, directory, processorCountLow, processorCountHi, L1Low, L1Hi)

if __name__ == "__main__":
    main()