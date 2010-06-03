add_library (suc
   BloomFilter.cpp
   CacheCore.cpp
   callback.cpp
   Config.cpp
   conflex.tab.cpp
   conflexlex.cpp
   GStats.cpp
   MSHR.cpp
   nanassert.cpp
   Port.cpp
   ReportGen.cpp
   ReportTherm.cpp
   SCTable.cpp
   SescConf.cpp
   Snippets.cpp
   ThermTrace.cpp
   TQueue.cpp
   TraceGen.cpp
   zlibStreams.cpp
)

set (EXTRA_LIBS ${EXTRA_LIBS} suc)
