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
#IN_EXT='.memDevResults.3200000'
#IN_EXT='-special.memDevResults'
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

def getGraphAverageResults(benchmarks, dirtypes, benchmarkResults, minimum, maximum,isNormalize):
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
    if (isNormalize):
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

def plotCpuLatencyMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2,isSaveFigure,isNormalize,filenameAddition):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'AverageLatency',
        'Average Latency (%)','cpu-latency-c'+l1+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotCpuLatencySingle(benchmarks, dirtypes, minimum, maximum, l1, l2,isSaveFigure,isNormalize,filenameAddition):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'AverageLatency', 'Average Latency (%)',
        'cpu-latency-c'+l1+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotCpuMessagesMultiple(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'TotalMessagesReceived',
        'Messages Received (%)','cpu-messages-c'+l1+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotCpuMessagesSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'Network', 'TotalMessagesReceived',
        'Messages Received (%)','cpu-messages-c'+l1+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotCpuTimeMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2, isSaveFigure,isNormalize,filenameAddition):
    plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2, 'TotalRunTime', 'TotalRunTime',
         'Runtime (%)','cpu-time-c'+l1+'-'+filenameAddition, isSaveFigure,isNormalize)

def plotCpuTimeSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,isSaveFigure,isNormalize,filenameAddition):
    plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'cpu-time-c'+l1+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotCpuMultiple(benchmarks, dirtypes, minimum, maximum, l1, l2, component, key, myYlabel, filename, isSaveFigure,isNormalize):
    myXlabel='Number of Processors'
    iResults = getCpuResults(benchmarks, dirtypes, minimum, maximum, l1, l2, component, key)
    plotGraphMultiple(benchmarks,dirtypes,iResults,minimum,maximum,myXlabel,myYlabel,filename,isSaveFigure,isNormalize)

def plotCpuSingle(benchmarks, dirtypes, minimum, maximum,l1,l2,component,key, myYlabel, filename,isSaveFigure,isNormalize):
    myXlabel='Number of Processors'
    iResults = getCpuResults(benchmarks, dirtypes, minimum, maximum, l1, l2, component, key)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes, iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNormalize)

def plotCpuAverage(benchmarks, dirtypes, mincpu, maxcpu, component, key,isNormalize):
    myXlabel='Number of Processors'
    myYlabel='Runtime (%)'
    cpuResults = getCpuResults(benchmarks, dirtypes, mincpu, maxcpu, DEFAULT_L1, DEFAULT_L2, component, key)
    myTitle = 'combined average'
    graphResults = getGraphAverageResults(benchmarks, dirtypes, cpuResults, mincpu, maxcpu,isNormalize)
    plotGraphSingle(dirtypes, graphResults, mincpu, maxcpu, myXlabel, myYlabel, myTitle)

def plotGraphMultiple(benchmarks,dirtypes,iResults,minimum,maximum,myXlabel,myYlabel,filename,isSaveFigure,isNormalize):
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
        plotGraphSingle(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle)
        #plotGraphSingle(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle, myGrid[figureIndex-1])
        benchmarkString += benchmark+'-'
        figureIndex += 1

    if (isSaveFigure):
        outfullpath = OUT_DIR+benchmarkString+filename+OUT_EXT
        savefig(outfullpath)
    else:
        show()
    close()

def plotGraphSingle(dirtypes, graphResults, minimum, maximum, myXlabel, myYlabel, myTitle):
    minInt = convertToInt(minimum)
    maxInt = convertToInt(maximum)
    linestyleIndex = 0
    linestyle = LINESTYLES[linestyleIndex]
    ticks = []
    yAxis = 0
    #if (myTitle!='radix'):
    if (True):
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
                if (y > yAxis):
                    yAxis= y
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
        # for radix only
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
    axis([0, maxInt*1.1, 0, yAxis*1.1])

def plotL1LatencyMultiple(benchmarks, dirtypes,cpu,minimum,maximum, l2,isSaveFigure,isNormalize,filenameAddition):
    plotL1Multiple(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'AverageLatency', 'Average Latency (%)',
    'l1-latency-p'+cpu+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotL1LatencySingle(benchmarks, dirtypes,cpu,minimum,maximum, l2,isSaveFigure,isNormalize,filenameAddition):
    plotL1Single(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'AverageLatency', 'Average Latency (%)',
    'l1-latency-p'+cpu+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotL1MessagesMultiple(benchmarks, dirtypes,cpu,minimum, maximum,l2,isSaveFigure,isNormalize,filenameAddition):
    plotL1Multiple(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'TotalMessagesReceived',
    'Messages Received (%)','l1-messages-p'+cpu+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotL1MessagesSingle(benchmarks, dirtypes,cpu,minimum, maximum,l2,isSaveFigure,isNormalize,filenameAddition):
    plotL1Single(benchmarks, dirtypes,cpu,minimum, maximum,l2,'Network', 'TotalMessagesReceived',
    'Messages Received (%)','l1-messages-p'+cpu+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotL1Multiple(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key, myYlabel, filename, isSaveFigure,isNormalize):
    myXlabel='Cache Size (KB)'
    iResults = getL1Results(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key)
    plotGraphMultiple(benchmarks,dirtypes,iResults,minimum,maximum,myXlabel,myYlabel,filename,isSaveFigure,isNormalize)

def plotL1Single(benchmarks, dirtypes, cpu, minimum, maximum, l2, component, key, myYlabel, filename, isSaveFigure,isNormalize):
    myXlabel='L1 Cache Size in Kilobytes'
    iResults = getL1Results(benchmarks, dirtypes, cpu,minimum, maximum, l2, component, key)

    for benchmark in benchmarks:
        myTitle = benchmark
        tempBenchmarks = [benchmark]
        plotGraphMultiple(tempBenchmarks,dirtypes,iResults, minimum, maximum, myXlabel, myYlabel,filename,isSaveFigure,isNormalize)

def plotL1TimeMultiple(benchmarks, dirtypes, cpu, minimum, maximum, l2, isSaveFigure,isNormalize,filenameAddition):
    plotL1Multiple(benchmarks, dirtypes, cpu, minimum, maximum, l2,'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'l1-time-p'+cpu+'-'+filenameAddition,isSaveFigure,isNormalize)

def plotL1TimeSingle(benchmarks, dirtypes, cpu, minimum, maximum, l2, isSaveFigure,isNormalize,filenameAddition):
    plotL1Single(benchmarks, dirtypes, cpu, minimum, maximum, l2,'TotalRunTime', 'TotalRunTime', 'Runtime (%)',
        'l1-time-p'+cpu+'-'+filenameAddition,isSaveFigure,isNormalize)


def main():
    #benchmarks = ['newtest','barnes', 'cholesky', 'fft', 'fmm', 'radix', 'raytrace', 'ocean']
    benchmarks = ['newtest']
    #benchmarks = ['fft', 'cholesky', 'ocean', 'radix']
    #dirtypes = ['bip', 'directory', 'origin']
    dirtypes = ['bip', 'origin']
    filenameAddition = '100-110'
    mincpu = '4'
    maxcpu = '32'
    minl1 = '1'
    #maxl1 = '128'
    maxl1 = '1'
    l2 = '1024'
    isNormalize = False
    isSaveFigure = True

    l1Index = int(minl1)
    while (l1Index <= int(maxl1)):
        #plotCpuTimeMultiple(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSaveFigure,isNormalize,filenameAddition)
        plotCpuTimeSingle(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSaveFigure,isNormalize,filenameAddition)
        #plotCpuMessagesMultiple(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSaveFigure,isNormalize,filenameAddition)
        plotCpuMessagesSingle(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSaveFigure,isNormalize,filenameAddition)
        #plotCpuLatencyMultiple(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSaveFigure,isNormalize,filenameAddition)
        plotCpuLatencySingle(benchmarks, dirtypes, mincpu, maxcpu,str(l1Index),l2,isSaveFigure,isNormalize,filenameAddition)
        l1Index *= 2
    cpuIndex = int(mincpu)
    while (cpuIndex <= int(maxcpu)):
        #plotL1TimeMultiple(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSaveFigure,isNormalize,filenameAddition)
        #plotL1TimeSingle(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSaveFigure,isNormalize,filenameAddition)
        #plotL1MessagesMultiple(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSaveFigure,isNormalize,filenameAddition)
        #plotL1MessagesSingle(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSaveFigure,isNormalize,filenameAddition)
        #plotL1LatencyMultiple(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSaveFigure,isNormalize,filenameAddition)
        #plotL1LatencySingle(benchmarks, dirtypes, str(cpuIndex), minl1, maxl1, l2, isSaveFigure,isNormalize,filenameAddition)
        cpuIndex *= 2

    #plotCpuMessages(benchmarks, dirtypes, mincpu, maxcpu)
    #plotCpuLatency(benchmarks, dirtypes, mincpu, maxcpu)
    #plotL1Single(benchmarks, dirtypes, minl1, maxl1, componentKey[0], componentKey[1])

if __name__ == "__main__":
    main()
