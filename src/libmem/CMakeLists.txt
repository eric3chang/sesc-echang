add_library ( mem
   AddressPrefetcher.cpp
   AlwaysPrefetch.cpp
   Bank.cpp
   Bus.cpp
   Cache.cpp
   MarkovPrefetcher.cpp
   MemCtrl.cpp
   MemoryOS.cpp
   MemorySystem_Old.cpp
   mtst1.cpp
   PriorityBus.cpp
   StridePrefetcher.cpp
   TLB.cpp
   UglyMemRequest.cpp
)

set (EXTRA_LIBS ${EXTRA_LIBS} libmem)

#add_executable (augSesc mtst1.cpp)
#target_link_libraries (augSesc ${EXTRA_LIBS})
