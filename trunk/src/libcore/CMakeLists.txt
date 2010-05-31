add_library (core
   AdvancedStats.cpp
   BPred.cpp
   Cluster.cpp
   DepWindow.cpp
   DInst.cpp
   FetchEngine.cpp
   GMemoryOS.cpp
   GMemorySystem.cpp
   GProcessor.cpp
   LDSTBuffer.cpp
   LDSTQ.cpp
   MemObj.cpp
   MemRequest.cpp
   OSSim.cpp
   Pipeline.cpp
   ProcessId.cpp
   Resource.cpp
   RiskLoadProf.cpp
   RunningProcs.cpp
   SMTProcessor.cpp
   ValueTable.cpp
   VPred.cpp
)

#set (EXTRA_LIBS ${EXTRA_LIBS} libcore)
#link_directories 
