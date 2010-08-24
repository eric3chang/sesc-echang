import os
import sys

oldFilename = sys.argv[1]
#oldFilename = 'genome-directory-02cpu.out.win-mbp'

oldfile = open(oldFilename, 'r')
newfile = open(oldFilename+'.new','w')
errfile = open(oldFilename+'.err','w')

addrArray = []
errAddrArray = []

lines = oldfile.readlines()

#1st pass - get addresses that contain OnLocalInvalidateResponse
for line in lines:
    if line.count('OnLocalInvalidateResponse'):
        splitLine = line.split();
        for block in splitLine:
            if block.count('addr'):
                # prevents duplicate addresses
                if not (addrArray.count(block)):
                    addrArray.append(block)
                    break

# 2nd pass - put all lines with suspicious addresses in one file
for addr in addrArray:
    for line in lines:
        if line.count(addr):
            newfile.write(line)
    newfile.write('\n')
newfile.close()

newfile = open(oldFilename+'.new','r')
# 3rd pass - put all addresses where OnLocalInvalidateResponse wasn't preceded
# immediately by a OnRemoteInvalidate in an array
thirdfileLines = newfile.readlines()
previousLine = ''
for currentLine in thirdfileLines:
    if currentLine.count('OnLocalInvalidateResponse') and \
        not previousLine.count('OnRemoteInvalidate'):
        splitLine = currentLine.split();
        for block in splitLine:
            if block.count('addr'):
                # prevent duplicate addresses
                if not (errAddrArray.count(block)):
                    errAddrArray.append(block)
                    break
    previousLine = currentLine

print(len(errAddrArray))

# 4th pass - put all addresses where OnLocalInvalidateResponse wasn't preceded
# immediately by a OnRemoteInvalidate in one file
for addr in errAddrArray:
    for line in thirdfileLines:
        if line.count(addr):
            errfile.write(line)
    errfile.write('\n')

oldfile.close()
newfile.close()
errfile.close()
raw_input()
