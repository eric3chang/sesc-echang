#!/users/erichang/csl/opt/bin/python
import os
import sys

from matplotlib.pyplot import *
#from mpl_toolkits.axes_grid1 import ImageGrid

# need to change this when moving this script
IN_DIR='../results/'
#OUT_DIR='./graphs/'
OUT_DIR='../../../report/generated-figures/'
#OUT_DIR='../configs/workFile/'

# don't need to change these when moving this script
CHOLESKY_INPUT='tk23'
#DPI=600
#LU_INPUT='b8'
#LU_INPUT='b16'
LU_INPUT='b32'
GRAPH_WIDTH=2
#IN_EXT='IN_EXT'
#LINESTYLES=['k--','k:']
LINESTYLES=['ko','k^']
#LINESTYLES=['k*','kD']
#LINESTYLES=['kp','kd']
LINESTYLE_SOLID='k-'
#OUT_EXT='.png'
OUT_EXT='.pdf'
#OUT_EXT='.eps'

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
                dictionary = getDictionary(benchmark, dirtype, str(cpu), l1, l2, IN_EXT)
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
                dictionary = getDictionary(benchmark, dirtype, cpu, str(i), l2, IN_EXT)
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

def getL2Results(benchmarks, dirtypes, cpu, l1, minimum, maximum, component, key):
    benchmarkResults = {}
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    for benchmark in benchmarks:
        dirtypeResults = {}
        for dirtype in dirtypes:
            iResults = {}
            i = minInt
            while (i <= maxInt):
                dictionary = getDictionary(benchmark, dirtype, cpu, l1, str(i), IN_EXT)
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

def getLatencyResults(benchmarks, dirtypes, cpu, l1, l2, minlatency, maxlatency, component, key):
    benchmarkResults = {}
    minlatencyInt = convertToInt(minlatency)
    maxlatencyInt = convertToInt(maxlatency)
    for benchmark in benchmarks:
        dirtypeResults = {}
        for dirtype in dirtypes:
            latencyResults = {}
            latency = minlatencyInt
            while (latency <= maxlatencyInt):
                fileAdd = '.localsendtime4-network'
                fileAdd += str(latency)
                #fileAdd += '0.newtest2'
                fileAdd += '0'
                inExt = '.memDevResults' + fileAdd
                dictionary = getDictionary(benchmark, dirtype, str(cpu), l1, l2, inExt)
                if component not in dictionary:
                    printError(component + ' should be in the file')
                else:
                    subDictionary = dictionary[component]
                if key not in subDictionary:
                    printError(key + ' should be in the file')
                else:
                    value = subDictionary[key]
                subDirectory = dictionary['Network']
                initialTime = subDirectory['initialTime']

                latencyResults[initialTime] = value
                latency += 1
            dirtypeResults[dirtype] = latencyResults
        benchmarkResults[benchmark] = dirtypeResults
    return benchmarkResults

def getDictionary(benchmark, dirtype, cpu, l1, l2, inExt):
    fullpath = getFilename(benchmark, dirtype, cpu, l1, l2, inExt)
    # replace cholesky's filename with correct one
    if (benchmark=='cholesky'):
        splitfullpath = fullpath.split('.')
        fullpath = ''
        for myWord in splitfullpath:
            fullpath += myWord + '.'
            if (myWord == 'memDevResults'):
                fullpath += CHOLESKY_INPUT + '.'
        fullpath = fullpath.rstrip('.')
    elif (benchmark=='lu'):
        splitfullpath = fullpath.split('.')
        fullpath = ''
        for myWord in splitfullpath:
            fullpath += myWord + '.'
            if (myWord == 'memDevResults'):
                fullpath += LU_INPUT + '.'
        fullpath = fullpath.rstrip('.')

    filein = open(fullpath, 'r')
    dictionary = {}
    line = ''
    for line in filein:
        line = line.strip()
        if (line.count(':')):
            splitlines = line.split(':')
            if (len(splitlines)!=3):
                print('splitlines=')
                print(splitlines)
                printError('input file format should have dictionary of 3, filein was ' + fullpath)
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

def getFilename(benchmark, dirtype, cpu, l1, l2, inExt):
    filename = benchmark+'-'+dirtype+'-'+cpu+'-'+l1+'-'+l2+ inExt
    fullpath = IN_DIR+filename
    return fullpath

def getGraphAverageResults(benchmarks, dirtypes, benchmarkResults, minimum, maximum,isNormalize):
    dirtypeResults = {}
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    maxValue = 0.0
    firstDirtypeSource = benchmarkResults[benchmarks[0]]
    firstISource = firstDirtypeSource[dirtypes[0]]
    allKeys = firstISource.keys()
    allKeys.sort()
    for dirtype in dirtypes:
        iResults = {}
        i = minInt
        for key in allKeys:
            total = 0
            for benchmark in benchmarks:
                dirtypeSources = benchmarkResults[benchmark]
                iSources = dirtypeSources[dirtype]
                value = iSources[key]
                total += float(value)
            average = total/len(benchmarks)
            iResults[key] = average
            if (average > maxValue):
                maxValue = average
        dirtypeResults[dirtype] = iResults
    if (isNormalize):
        # normalize values
        for dirtype in dirtypes:
            iSources = dirtypeResults[dirtype]
            keys = iSources.keys()
            for key in keys:
                value = iSources[key]
                value /= maxValue
                value *= 100
                iSources[key] = value
    return dirtypeResults

def plotCpuCacheLatencySingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'TotalDirectory', 'averageWaitTime', 'Latency (%)',
        'cpu-cachelatency-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuCacheLatencySimpleSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,fileAdd,isSwitchDirtype):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'TotalDirectory', 'averageWaitTimeSimple', 'Latency (%)',
        'cpu-cachelatency-c'+l2+fileAdd,isSaveFigure,isNormalize,isSwitchDirtype)


def plotCpuCacheTotalLatencySingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,fileAdd,isSwitchDirtype):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'TotalDirectory', 'totalLatency', 'Latency (%)',
        'cpu-cachetotallatency-c'+l2+fileAdd,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuCacheTotalLatencySimpleSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,fileAdd,isSwitchDirtype):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'TotalDirectory', 'totalLatencySimple', 'Latency (%)',
        'cpu-cachetotallatency-c'+l2+fileAdd,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuNetworkLatencyMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'AverageLatency',
        'Average Latency (%)','cpu-networklatency-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuNetworkLatencySingle(benchmarks, dirtypes, minimum, maximum, l1, l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'AverageLatency', 'Average Latency (%)',
        'cpu-networklatency-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuMessagesMultiple(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    messagesReceived=''
    if (isNormalize):
        messagesReceived='Messages Received (%)'
    else:
        messagesReceived='Messages Received'
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'TotalMessagesReceived',
        messagesReceived,'cpu-messages-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuMessagesSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    messagesReceived=''
    if (isNormalize):
        messagesReceived='Messages Received (%)'
    else:
        messagesReceived='Messages Received'
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'TotalMessagesReceived',
        messagesReceived,'cpu-messages-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuTimeMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2, isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2, 'TotalRunTime', 'TotalRunTime',
         'Runtime (%)','cpu-time-c'+l2+filenameAddition, isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuTimeSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'cpu-time-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuL2Misses(benchmarks, dirtypes, minimum, maximum, l1, l2, isSaveFigure, isNorm, filenameAdd, isSwitchDirtype):
    myXlabel='Number of Processors'
    myYlabel=''
    if (isNorm):
        myYlabel='L2 Misses (%)'
    else:
        myYlabel='L2 Misses'
    filename='cpu-l2miss-c'+l2+filenameAdd
    exclusiveReadMisses = getCpuResults(benchmarks, dirtypes, minimum,maximum, l1, l2, 'TotalCache', 'totalL2CacheExclusiveReadMisses')
    sharedReadMisses = getCpuResults(benchmarks, dirtypes, minimum, maximum, l1, l2, 'TotalCache', 'totalL2CacheSharedReadMisses')
    iResults=combineResults(benchmarks, dirtypes, minimum, maximum, l1, l2, exclusiveReadMisses, sharedReadMisses)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes, iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNorm,isSwitchDirtype)

def combineResults(benchmarks, dirtypes, minimum, maximum, l1, l2, exclusiveReadMisses, sharedReadMisses):
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    for benchmark in benchmarks:
        exclusiveReadSources = exclusiveReadMisses[benchmark]
        sharedReadSources = sharedReadMisses[benchmark]
        for dirtype in dirtypes:
            exclusiveDirtypeSources = exclusiveReadSources[dirtype]
            sharedDirtypeSources = sharedReadSources[dirtype]
            i = minInt
            while (i <= maxInt):
                myInt = 0
                myInt = int(exclusiveDirtypeSources[i]) + int(sharedDirtypeSources[i])
                exclusiveDirtypeSources[i] = str(myInt)
                i *= 2
            exclusiveReadSources[dirtype] = exclusiveDirtypeSources
    return exclusiveReadMisses

def plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2, component, key, myYlabel, filename, isSaveFigure,isNormalize,isSwitchDirtype):
    myXlabel='Number of Processors'
    iResults = getCpuResults(benchmarks, dirtypes, minimum, maximum, l1, l2, component, key)
    plotGraphMultiple(benchmarks,dirtypes,iResults,minimum,maximum,myXlabel,myYlabel,filename,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,component,key, myYlabel, filename,isSaveFigure,isNormalize,isSwitchDirtype):
    myXlabel='Number of Processors'
    iResults = getCpuResults(benchmarks, dirtypes, minimum, maximum, l1, l2, component, key)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes, iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNormalize,isSwitchDirtype)

def plotCpuAverage(benchmarks, dirtypes, mincpu, maxcpu, component, key,isNormalize):
    myXlabel='Number of Processors'
    myYlabel='Runtime (%)'
    cpuResults = getCpuResults(benchmarks, dirtypes, mincpu, maxcpu, DEFAULT_L1, DEFAULT_L2, component, key)
    myTitle = 'combined average'
    graphResults = getGraphAverageResults(benchmarks, dirtypes, cpuResults, mincpu, maxcpu,isNormalize)
    plotGraphSingle(dirtypes, graphResults, mincpu, maxcpu, myXlabel, myYlabel, myTitle,isSwitchDirtypes)

def plotLatencySingle(benchmarks, dirtypes, cpu,l1,l2, minimum, maximum, component,key, myYlabel, filename,isSaveFigure,isNormalize,isSwitchDirtype):
    myXlabel='Network Latency (Cycles)'
    iResults1 = getLatencyResults(['writetest'], dirtypes, cpu, l1, l2, minimum, maximum, component, key)
    iResults2 = getLatencyResults(['readtest'], dirtypes, cpu, l1, l2, minimum, maximum, component, key)

    iResults = subtractResults('writetest', iResults1, 'readtest', iResults2, dirtypes, cpu, l1, l2, minimum, maximum)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes, iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNormalize,isSwitchDirtype)

def plotLatencyTimeSingle(benchmarks, dirtypes, cpu,l1,l2, minlatency, maxlatency, isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotLatencySingle(benchmarks, dirtypes, cpu,l1,l2, minlatency, maxlatency, 'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'latency-time-p' + cpu + '-c'+l2+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL1NetworkLatencyMultiple(benchmarks, dirtypes,cpu,minimum,maximum, l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL1Multiple(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'AverageLatency', 'Average Latency (%)',
    'l1-latency-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL1NetworkLatencySingle(benchmarks, dirtypes,cpu,minimum,maximum, l2,isSaveFigure,isNormalize,filenameAddition, isSwitchDirtype):
    plotL1Single(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'AverageLatency', 'Average Latency (%)',
    'l1-latency-p'+cpu+filenameAddition,isSaveFigure,isNormalize, isSwitchDirtype)

def plotL1MessagesMultiple(benchmarks, dirtypes,cpu,minimum, maximum,l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL1Multiple(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'TotalMessagesReceived',
    'Messages Received (%)','l1-messages-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL1MessagesSingle(benchmarks, dirtypes,cpu,minimum, maximum,l2,isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL1Single(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'TotalMessagesReceived',
    'Messages Received (%)','l1-messages-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL1Multiple(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key, myYlabel, filename, isSaveFigure,isNormalize,
        isSwitchDirtype):
    myXlabel='Cache Size (KB)'
    iResults = getL1Results(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key)
    plotGraphMultiple(benchmarks,dirtypes,iResults,minimum,maximum,myXlabel,myYlabel,filename,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL1Single(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key, myYlabel, filename, isSaveFigure,isNormalize,
        isSwitchDirtype):
    myXlabel='L1 Cache Size in Kilobytes'
    iResults = getL1Results(benchmarks, dirtypes, cpu,minimum, maximum, l2, component, key)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes,iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNormalize,
            isSwitchDirtype)

def plotL1TimeMultiple(benchmarks, dirtypes, cpu, minimum, maximum, l2, isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL1Multiple(benchmarks, dirtypes, cpu, minimum, maximum, l2,'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'l1-time-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL1TimeSingle(benchmarks, dirtypes, cpu, minimum, maximum, l2, isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL1Single(benchmarks, dirtypes, cpu, minimum, maximum, l2,'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'l1-time-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL2CacheLatencySingle(benchmarks, dirtypes,cpu, l1, minimum,maximum, isSaveFigure,isNormalize,filenameAddition, isSwitchDirtype):
    plotL2Single(benchmarks, dirtypes,cpu, l1, minimum, maximum, 'TotalDirectory', 'averageWaitTime', 'Latency (%)',
    'l2-cachelatency-p'+cpu+filenameAddition,isSaveFigure,isNormalize, isSwitchDirtype)

def plotL2NetworkLatencySingle(benchmarks, dirtypes,cpu, l1, minimum,maximum, isSaveFigure,isNormalize,filenameAddition, isSwitchDirtype):
    plotL2Single(benchmarks, dirtypes,cpu, l1, minimum, maximum, 'Network', 'AverageLatency', 'Average Latency (%)',
    'l2-networklatency-p'+cpu+filenameAddition,isSaveFigure,isNormalize, isSwitchDirtype)

def plotL2MessagesSingle(benchmarks, dirtypes,cpu, l1, minimum, maximum, isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL2Single(benchmarks, dirtypes,cpu, l1, minimum, maximum, 'Network', 'TotalMessagesReceived',
    'Messages Received (%)','l2-messages-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotL2Single(benchmarks, dirtypes, cpu, l1, minimum, maximum, component, key, myYlabel, filename, isSaveFigure,isNormalize,
        isSwitchDirtype):
    myXlabel='L2 Cache Size in Kilobytes'
    iResults = getL2Results(benchmarks, dirtypes, cpu, l1, minimum, maximum, component, key)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes,iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNormalize,
            isSwitchDirtype)

def plotL2TimeSingle(benchmarks, dirtypes, cpu, l1, minimum, maximum, isSaveFigure,isNormalize,filenameAddition,isSwitchDirtype):
    plotL2Single(benchmarks, dirtypes, cpu, l1, minimum, maximum, 'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'l2-time-p'+cpu+filenameAddition,isSaveFigure,isNormalize,isSwitchDirtype)

def plotGraphMultiple(benchmarks,dirtypes,iResults,minimum,maximum,myXlabel,myYlabel,filename,isSaveFigure,isNormalize,
        isSwitchDirtypes):
    benchmarkLength = len(benchmarks)
    row = benchmarkLength / GRAPH_WIDTH
    row = int(row)
    remainder = benchmarkLength % GRAPH_WIDTH
    # if remainder > 0, we need an extra row to accomodate all the graphs that will be drawn
    if (remainder > 0):
        row += 1

    if (benchmarkLength > GRAPH_WIDTH):
        column = GRAPH_WIDTH
    else:
        column = benchmarkLength

    figureIndexPrefix = str(row) + str(column)

    #myFigure = figure(1)
    #myGrid = ImageGrid(myFigure, 111, nrows_ncols = (row, column), axes_pad=0.1)
    subplots_adjust(hspace=0.4)
    subplots_adjust(wspace=0.4)

    figureIndex = 1
    benchmarkString = ''
    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        graphResults = getGraphAverageResults(tempBenchmarks, dirtypes, iResults, minimum, maximum,isNormalize)
        subplotIn = figureIndexPrefix + str(figureIndex)
        subplot(subplotIn)
        plotGraphSingle(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle,isSwitchDirtypes)
        #plotGraphSingle(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle, myGrid[figureIndex-1])
        benchmarkString += benchmark+'-'
        figureIndex += 1

    if (isSaveFigure):
        outfullpath = ''
        if (benchmarkString=='lu-'):
            outfullpath = OUT_DIR+benchmarkString+filename+'.'+LU_INPUT+OUT_EXT
        else:
            outfullpath = OUT_DIR+benchmarkString+filename+OUT_EXT
        #savefig(outfullpath, dpi=DPI)
        savefig(outfullpath)
    else:
        show()
    close()

def plotGraphSingle(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle,isSwitchDirtypes):
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    linestyleIndex = 0
    linestyle = LINESTYLES[linestyleIndex]
    ticks = []
    yAxis = 0
    if (not isSwitchDirtypes):
        for dirtype in dirtypes:
            iSources = graphResults[dirtype]
            keys = iSources.keys()
            keys.sort()
            xValues = []
            yValues = []
            for myKey in keys:
                x = myKey
                y = iSources[myKey]
                xValues.append(x)
                yValues.append(y)
                if (y > yAxis):
                    yAxis= y
                ticks.append(x)
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
                if (y > yAxis):
                    yAxis = y
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
    #xticks(ticks)
    grid(True)
    #title(myTitle)
    title('synthetic benchmark')
    #axis([0, maxInt*1.1, 0, yAxis*1.1])

def subtractResults(benchmark1, results1, benchmark2, results2, dirtypes, cpu, l1, l2, minimum, maximum):
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)

    sources1 = results1[benchmark1]
    sources2 = results2[benchmark2]
    for dirtype in dirtypes:
        dirtypeSources1 = sources1[dirtype]
        dirtypeSources2 = sources2[dirtype]
        keys = dirtypeSources1.keys()
        keys.sort()
        for key in keys:
            myInt = 0
            myInt = int(dirtypeSources1[key]) - int(dirtypeSources2[key])
            dirtypeSources1[key] = str(myInt)
        sources1[dirtype] = dirtypeSources1
    results1[benchmark1] = sources1
    return results1

def main():
    #benchmarks = ['cholesky', 'fft', 'lu','newtest', 'radix', 'ocean']
    #benchmarks = ['cholesky', 'fft', 'newtest', 'radix', 'ocean']
    #benchmarks = ['readtest']
    benchmarks = ['writetest']
    dirtypes = ['bip', 'origin']
    #bipdirtypes = ['bip']
    #origindirtypes = ['origin']
    cpu = '4'
    mincpu = '4'
    maxcpu = '32'
    l1 = '64'
    l2 = '512'
    #minl2 = '128'
    #minl2 = '512'
    #maxl2 = '4096'
    minl2 = '1024'
    maxl2 = '1024'
    minlatency = '0'
    maxlatency = '7'
    isNorm = False
    isSavFig = False
    isSwitchDir = False
    global IN_EXT
    fileAdd = ''
    #fileAdd = '.network05'
    #fileAdd = '.network10'
    #fileAdd = '.newtest20'
    #fileAdd = '.network90'
    #fileAdd = '.localsendtime4.network05'
    #fileAdd = '.localsendtime60.network05'
    IN_EXT = '.memDevResults' + fileAdd

    plotLatencyTimeSingle(benchmarks, dirtypes, cpu, l1, l2, minlatency, maxlatency, isSavFig,isNorm,fileAdd,isSwitchDir)

    l2Index = int(minl2)
    while (l2Index <= int(maxl2)):
        #plotCpuTimeSingle(benchmarks, dirtypes, mincpu, maxcpu, l1, str(l2Index),isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuMessagesSingle(benchmarks, dirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuL2Misses(benchmarks, dirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig, isNorm, fileAdd, isSwitchDir)

        #plotCpuTimeMultiple(benchmarks, dirtypes, mincpu, maxcpu, l1, str(l2Index),isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuNetworkLatencySingle(benchmarks, dirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuCacheLatencySingle(benchmarks, origindirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuCacheLatencySimpleSingle(benchmarks, bipdirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuCacheTotalLatencySingle(benchmarks, origindirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuCacheTotalLatencySimpleSingle(benchmarks, bipdirtypes, mincpu, maxcpu, l1, str(l2Index), isSavFig,isNorm,fileAdd,isSwitchDir)
        l2Index *= 2

    cpuIndex = int(mincpu)
    while (cpuIndex <= int(maxcpu)):
        #plotL2TimeSingle(benchmarks, dirtypes, str(cpuIndex), l1, minl2, maxl2, isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotL2MessagesSingle(benchmarks, dirtypes, str(cpuIndex), l1, minl2, maxl2, isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotL2NetworkLatencySingle(benchmarks, dirtypes, str(cpuIndex), l1, minl2, maxl2, isSavFig,isNorm,fileAdd, isSwitchDir)
        #plotL2CacheLatencySingle(benchmarks, dirtypes, str(cpuIndex), l1, minl2, maxl2, isSavFig,isNorm,fileAdd, isSwitchDir)
        cpuIndex *= 2

    '''
    cpuIndex = int(mincpu)
    while (cpuIndex <= int(maxcpu)):
        #plotL1TimeMultiple(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSavFig,isNorm,fileAdd,isSwitchDir)
        plotL1TimeSingle(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotL1MessagesMultiple(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSavFig,isNorm,fileAdd,isSwitchDir)
        plotL1MessagesSingle(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotL1NetworkLatencyMultiple(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSavFig,isNorm,fileAdd, isSwitchDir)
        plotL1NetworkLatencySingle(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSavFig,isNorm,fileAdd, isSwitchDir)
        cpuIndex *= 2
    '''

    '''
    l1Index = int(minl1)
    while (l1Index <= int(maxl1)):
        #plotCpuTimeMultiple(benchmarks, dirtypes, mincpu, maxcpu, str(l1Index), l2, isSavFig,isNorm,fileAdd,isSwitchDir)
        plotCpuTimeSingle(benchmarks, dirtypes, mincpu, maxcpu, str(l1Index), l2, isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuMessagesMultiple(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSavFig,isNorm,fileAdd,isSwitchDir)
        plotCpuMessagesSingle(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSavFig,isNorm,fileAdd,isSwitchDir)
        #plotCpuNetworkLatencyMultiple(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSavFig,isNorm,fileAdd,isSwitchDir)
        plotCpuNetworkLatencySingle(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSavFig,isNorm,fileAdd,isSwitchDir)
        l1Index *= 2
    '''

    #plotCpuMessages(benchmarks, dirtypes, mincpu, maxcpu)
    #plotCpuNetworkLatency(benchmarks, dirtypes, mincpu, maxcpu)
    #plotL1Single(benchmarks, dirtypes, minl1, maxl1, componentKey[0], componentKey[1])

if __name__ == "__main__":
    main()
