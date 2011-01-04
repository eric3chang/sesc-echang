#!/usr/bin/python
import sys

NEWFILENAME='msg-id-count.out'

if (len(sys.argv)!=2):
    print ('need more arguments')
    sys.exit()

oldfile = open(sys.argv[1])
newfile = open(NEWFILENAME, 'w')

messageIDCounts = {}

for line in oldfile:
    words = line.split()
    isMsgID = False
    for word in words:
        if (isMsgID):
            if (messageIDCounts.has_key(word)):
                messageIDCounts[word] += 1
            else:
                messageIDCounts[word] = 1;
            isMsgID = False
        if word.count('msg='):
            isMsgID = True

reverseMessageIDCounts = {}

for messageID in messageIDCounts:
    #print(messageID, messageIDCounts[messageID])
    if (not reverseMessageIDCounts.has_key(messageIDCounts[messageID])):
        reverseMessageIDCounts[messageIDCounts[messageID]] = [];
        reverseMessageIDCounts[messageIDCounts[messageID]].append(messageID);
    else:
        reverseMessageIDCounts[messageIDCounts[messageID]].append(messageID);

for messageIDCounts in reverseMessageIDCounts:
    newfile.write(str(messageIDCounts))
    newfile.write(str(reverseMessageIDCounts[messageIDCounts]))
    newfile.write('\n\n')

oldfile.close()
newfile.close()