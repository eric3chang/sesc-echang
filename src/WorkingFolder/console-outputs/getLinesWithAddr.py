#!/usr/bin/python
import os
import sys

if (len(sys.argv) < 3):
   print('Usage: getLinesWithAddr.py filename methodname')
   sys.exit()

oldFilename = sys.argv[1]
methodname = sys.argv[2]
newFilename = oldFilename+'.'+methodname
#oldFilename = 'genome-directory-02cpu.out.win-mbp'

oldfile = open(oldFilename, 'r')
newfile = open(newFilename,'w')

addrArray = []
errAddrArray = []

lines = oldfile.readlines()

#1st pass - get addresses that contain methodname
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

if os.path.exists('/usr/bin/dos2unix'):
   os.system('dos2unix '+newFilename)
elif os.path.exists('/usr/bin/fromdos'):
   os.system('fromdos '+newFilename)
else:
   print('dos2unix and fromdos not found: not converting file from dos')

# close files
oldfile.close()
newfile.close()
raw_input('press any key to continue...')
