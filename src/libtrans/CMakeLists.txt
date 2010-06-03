add_library (trans
    CollisionDetectionExcludeAllBut.cpp
    LogTMCollisionDetection.cpp
    LogTMSECollisionDetection.cpp
    PerfectCollisionDetection.cpp
    PerfectCollisionManager.cpp
    PerfectVersioning.cpp
    TMInterface.cpp
    TMMemory.cpp
    TMProcessor.cpp
)

set (EXTRA_LIBS ${EXTRA_LIBS} trans)
