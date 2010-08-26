#!/usr/bin/python
import os
import sys

if (len(sys.argv) < 3):
   print('Usage: getLinesWithAddr.py filename methodname')
   sys.exit()

oldFilename = sys.argv[1]
methodname = sys.argv[2]
#oldFilename = 'genome-directory-02cpu.out.win-mbp'

oldfile = open(oldFilename, 'r')
newfile = open(oldFilename+'.new','w')

addrArray = []
errAddrArray = []

lines = oldfile.readlines()

#1st pass - get addresses that contain OnLocalInvalidateResponse
for line in lines:
    if line.count(methodname):
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

'''
newfile = open(oldFilename+'.new','r')
# 3rd pass - put all addresses where methodname wasn't preceded
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

errfile = open(oldFilename+'.err','w')
# 4th pass - put all addresses where OnLocalInvalidateResponse wasn't preceded
# immediately by a OnRemoteInvalidate in one file
for addr in errAddrArray:
    for line in thirdfileLines:
        if line.count(addr):
            errfile.write(line)
    errfile.write('\n')
errfile.close()
'''

# close files
oldfile.close()
newfile.close()
raw_input('press any key to continue...')
