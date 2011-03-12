#!/bin/bash
# generates new test_xxxcpu.conf files given the number of cpus

CPU_COUNT=128
NEW_FILE=test_128cpu.conf

rm -f $NEW_FILE

echo "nCPUs = $CPU_COUNT" >> $NEW_FILE

index=0
while [[ $index -lt $CPU_COUNT ]]
do
   echo "cpucore[$index] = 'ProcDesc$index'" >> $NEW_FILE
   let index++
done

echo "" >> $NEW_FILE
echo "<shared.conf>" >> $NEW_FILE
echo "" >> $NEW_FILE

index=0
while [[ $index -lt $CPU_COUNT ]]
do
   echo "[ProcDesc$index]" >> $NEW_FILE
   echo "archBits       = 32" >> $NEW_FILE
   echo "areaFactor     = (\$(issue)*\$(depth)+0.1)/16  # Area in relation with alpha264 EV6" >> $NEW_FILE
   echo "issueWrongPath = true" >> $NEW_FILE
   echo "inorder        = false" >> $NEW_FILE
   echo "fetchWidth     = (\$(issue)/6+1)*6" >> $NEW_FILE
   echo "instQueueSize  = 2*(\$(issue)/6+1)*6" >> $NEW_FILE
   echo "issueWidth     = \$(issue)" >> $NEW_FILE
   echo "retireWidth    = \$(issue)" >> $NEW_FILE
   echo "decodeDelay    = 3" >> $NEW_FILE
   echo "renameDelay    = 3" >> $NEW_FILE
   echo "maxBranches    = 4+6*\$(depth)" >> $NEW_FILE
   echo "bb4Cycle       = 1" >> $NEW_FILE
   echo "bpredDelay     = 1" >> $NEW_FILE
   echo "maxIRequests   = 3  # +1 icache hit delay -> 1 outs miss" >> $NEW_FILE
   echo "interClusterLat= 2  # P4 intra +1?" >> $NEW_FILE
   echo "cluster[0]     = 'FXClusterIssueX'" >> $NEW_FILE
   echo "cluster[1]     = 'FPClusterIssueX'" >> $NEW_FILE
   echo "stForwardDelay = 1  # +1 clk from the instruction latency" >> $NEW_FILE
   echo "maxLoads       = 6*\$(depth)+30" >> $NEW_FILE
   echo "maxStores      = 4*\$(depth)+30" >> $NEW_FILE
   echo "LSQBanks       = 1  # Banked LD/ST Queue " >> $NEW_FILE
   echo "regFileDelay   = 3" >> $NEW_FILE
   echo "robSize        = 26*\$(depth)+48" >> $NEW_FILE
   echo "intRegs        = 48+14*\$(depth)" >> $NEW_FILE
   echo "fpRegs         = 32+12*\$(depth)" >> $NEW_FILE
   echo "bpred          = 'BestBPred'" >> $NEW_FILE
   echo "dataSource     = 'ProcessorInterface_$index DL1'" >> $NEW_FILE
   echo "instrSource    = 'InstSource IL1'" >> $NEW_FILE
   echo "enableICache   = true" >> $NEW_FILE
   echo "dtlb           = 'FXDTLB'" >> $NEW_FILE
   echo "itlb           = 'FXITLB'" >> $NEW_FILE
   echo "OSType         = 'std'" >> $NEW_FILE
   echo "minTLBMissDelay= 16 # Min Clk to services a TLB miss (I/D)" >> $NEW_FILE
   echo "" >> $NEW_FILE
   echo "[ProcessorInterface_$index]" >> $NEW_FILE
   echo "deviceType   =  'SESCProcessorInterface'" >> $NEW_FILE
   echo "blockName   = 'Dcache'" >> $NEW_FILE
   echo "MSHR         = 'DL1MSHR'" >> $NEW_FILE
   echo "size         = 1024" >> $NEW_FILE
   echo "assoc        =  4" >> $NEW_FILE
   echo "skew         = false" >> $NEW_FILE
   echo "bsize        = 64" >> $NEW_FILE
   echo "replPolicy   = 'RANDOM'" >> $NEW_FILE
   echo "numPorts     = \$(memSizing)" >> $NEW_FILE
   echo "portOccp     =  1" >> $NEW_FILE
   echo "hitDelay     =  2" >> $NEW_FILE
   echo "missDelay    =  1" >> $NEW_FILE
   echo "writePolicy  = 'WB'" >> $NEW_FILE
   echo "lowerLevel   = 'voidDevice'" >> $NEW_FILE
   echo "DeviceSubtype = 'MOESICache'" >> $NEW_FILE
   echo "PortCount    = 1" >> $NEW_FILE
   echo "PortName[0]   = 'Down'" >> $NEW_FILE
   echo "DeviceID     = 1024+$index" >> $NEW_FILE
   echo "Port_0[0] = 2048+$index" >> $NEW_FILE
   echo "Port_0[1] = 0" >> $NEW_FILE
   echo "NodeID = $index" >> $NEW_FILE
   echo "Level = 1" >> $NEW_FILE
   echo "" >> $NEW_FILE
   let index++
done

echo "[InstSource]" >> $NEW_FILE
echo "deviceType  = 'niceCache'" >> $NEW_FILE
echo "size        = 1024" >> $NEW_FILE
echo "assoc       =    1" >> $NEW_FILE
echo "bsize       =   64" >> $NEW_FILE
echo "writePolicy = 'WB'" >> $NEW_FILE
echo "replPolicy  = 'LRU'" >> $NEW_FILE
echo "numPorts    =     1" >> $NEW_FILE
echo "portOccp    =     1" >> $NEW_FILE
echo "hitDelay    =   1   # 5GHz: ~100ns is 500 cycles (10 L1 + L2 miss)" >> $NEW_FILE
echo "missDelay   =   1600000 # Not valid" >> $NEW_FILE
echo "MSHR        = 'BigMemMSHR'" >> $NEW_FILE
echo "lowerLevel  = 'voidDevice'" >> $NEW_FILE
echo "" >> $NEW_FILE

