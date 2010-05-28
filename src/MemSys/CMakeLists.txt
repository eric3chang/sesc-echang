add_library (MemSys
   Connection.cpp
   EventManager.cpp
   MemorySystem.cpp
)

add_subdirectory (Devices)

set (EXTRA_LIBS ${EXTRA_LIBS} MemSys)
