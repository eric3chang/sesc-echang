add_library (Devices
   BaseMemDevice.cpp
   MOESICache.cpp
   SESCProcessorInterface.cpp
   SnoopyBus.cpp
   TestMemory.cpp
)

set (EXTRA_LIBS ${EXTRA_LIBS} Devices)
