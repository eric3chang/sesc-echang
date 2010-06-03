add_library (ll
   Events.cpp
   ExecutionFlow.cpp
   GFlow.cpp
   HeapManager.cpp
   Instruction.cpp
   MIPSInstruction.cpp
   PPCInstruction.cpp
   QemuSescReader.cpp
   ThreadContext.cpp
)

set (EXTRA_LIBS ${EXTRA_LIBS} ll)
