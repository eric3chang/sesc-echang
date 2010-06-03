cmake_minimum_required (VERSION 2.6)
project (augSesc)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/sescConfig.h.in"
  "${PROJECT_BINARY_DIR}/sescConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find all the header files
# uncommented the ones that we don't need
#include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/libapp")
include_directories ("${PROJECT_SOURCE_DIR}/libcore")
#include_directories ("${PROJECT_SOURCE_DIR}/libemul")
include_directories ("${PROJECT_SOURCE_DIR}/libll")
include_directories ("${PROJECT_SOURCE_DIR}/libmem")
include_directories ("${PROJECT_SOURCE_DIR}/libmint")
include_directories ("${PROJECT_SOURCE_DIR}/libnet")
include_directories ("${PROJECT_SOURCE_DIR}/libpint")
#include_directories ("${PROJECT_SOURCE_DIR}/libpower")
#include_directories ("${PROJECT_SOURCE_DIR}/librst")
#include_directories ("${PROJECT_SOURCE_DIR}/libsescspot")
include_directories ("${PROJECT_SOURCE_DIR}/libsmp")
include_directories ("${PROJECT_SOURCE_DIR}/libsuc")
#include_directories ("${PROJECT_SOURCE_DIR}/libsuperlu")
#include_directories ("${PROJECT_SOURCE_DIR}/libtm")
include_directories ("${PROJECT_SOURCE_DIR}/libtrans")
#include_directories ("${PROJECT_SOURCE_DIR}/libvmem")
include_directories ("${PROJECT_SOURCE_DIR}/MemSys")
include_directories ("${PROJECT_SOURCE_DIR}/MemSys/Devices")
include_directories ("${PROJECT_SOURCE_DIR}/MemSys/Messages")
include_directories ("${PROJECT_SOURCE_DIR}/include")

# add definitions to compiler
# have to do this before add_subdirectory
IF (UNIX)
   ADD_DEFINITIONS (-Wno-deprecated)
   ADD_DEFINITIONS (-DHANGING_ASERT)
   ADD_DEFINITIONS (-D_DEBUG)
   ADD_DEFINITIONS (-DSYSTEM_SESC)
   ADD_DEFINITIONS (-DDEBUG)
   ADD_DEFINITIONS (-D_CONSOLE)
   ADD_DEFINITIONS (-DLENDIAN)
   ADD_DEFINITIONS (-DDEBUG_DUMP_PROGRAM)
   ADD_DEFINITIONS (-DTRANSACTIONAL_MEMORY)
   ADD_DEFINITIONS (-DTRANSACTIONAL_ONLY)
   ADD_DEFINITIONS (-DADDRESS_CHK)
   ADD_DEFINITIONS (-DTRANSACTIONAL_COMPOSITION_TRACKING)
   #  the following definition doesn't work in older gnu compilers
   #  enable it to use new standards unordered_map and unordered_set
   #   ADD_DEFINITIONS (-std=c++0x)
ENDIF(UNIX)

# add compilation of the necessary directories of sesc
add_subdirectory (libcore)
add_subdirectory (libll)
add_subdirectory (libmem)
add_subdirectory (libmint)
add_subdirectory (libpint)
add_subdirectory (libsuc)
add_subdirectory (libtrans)
add_subdirectory (MemSys)

#set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)

# add the executable
target_link_libraries (augSesc ${EXTRA_LIBS})
#add_executable (augSesc "${PROJECT_SOURCE_DIR}/libmem/mtst1.cpp")

