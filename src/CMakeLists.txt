cmake_minimum_required (VERSION 2.6)

# added debug into the definitions below
if (UNIX)
   # if the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
   # definition and dump it in the cache, otherwise set CMAKE_BUILD_TYPE to Debug prior to calling PROJECT()
   if (NOT DEFINED CMAKE_BUILD_TYPE)
      set (CMAKE_BUILD_TYPE Debug CACHE STRING
         "Choose the type of build, options are: None Debug Release")
   endif (NOT DEFINED CMAKE_BUILD_TYPE)
endif(UNIX)

project (augSescProject)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/sescConfig.h.in"
  "${PROJECT_SOURCE_DIR}/sescConfig.h"
  )

# overwrite default DEBUG build
# ADDRESS_CHK - performs address alignment checks on individual memory accesses
set ( CMAKE_CXX_FLAGS_DEBUG "-ggdb -DDEBUG -DADDRESS_CHK -Wno-unused-variable" CACHE STRING
   "Flags used by the C++ compiler during debug builds."
   FORCE )
MARK_AS_ADVANCED (
   CMAKE_CXX_FLAGS_DEBUG )

# overwrite default Release build
# using -O2 or -O3 causes program to crash unexpectedly
# using -O1 possibly causes radix to crash
#set ( CMAKE_CXX_FLAGS_RELEASE "-DADDRESS_CHK -Wno-unused-variable" CACHE STRING
set ( CMAKE_CXX_FLAGS_RELEASE "-DADDRESS_CHK -Wno-unused-variable -O1" CACHE STRING
   "Flags used by the C++ compiler during release builds."
   FORCE )
MARK_AS_ADVANCED (
   CMAKE_CXX_FLAGS_RELEASE )

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
   ADD_DEFINITIONS (-Wall)
   ADD_DEFINITIONS (-Wno-deprecated)
   # causes assertions to halt the program instead of exit the program
   ADD_DEFINITIONS (-DHANGING_ASSERT)
   ADD_DEFINITIONS (-DSYSTEM_SESC)
   ADD_DEFINITIONS (-DLENDIAN)
   ADD_DEFINITIONS (-m32)
   #ADD_DEFINITIONS (-march=i386)
   #ADD_DEFINITIONS (-mtune=i386)

   # turned off
   # dumps program binary in a readable state upon startup
   # ADD_DEFINITIONS (-DDEBUG_DUMP_PROGRAM)
   # transactional memory only
   #ADD_DEFINITIONS (-DTRANSACTIONAL_COMPOSITION_TRACKING)
   #ADD_DEFINITIONS (-DTRANSACTIONAL_MEMORY)
   #ADD_DEFINITIONS (-DTRANSACTIONAL_ONLY)

   # vs specific
   #ADD_DEFINITIONS (-D_CONSOLE)
   #ADD_DEFINITIONS (-D_DEBUG)

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
# no longer need to add WorkingFolder because we put the configurations out of it
#add_subdirectory (WorkingFolder)

# set executable path
set (EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../WorkingFolder"
   CACHE INTERNAL "Non-configurable executable directory.")

# add all the libraries so that the linker can find them
set (EXTRA_LIBS ${EXTRA_LIBS} core ll mem mint pint suc trans MemSys Devices)

# add the executable, the executable should be put into WorkingFolder automatically
add_executable (augSesc "${PROJECT_SOURCE_DIR}/libmem/mtst1.cpp")
target_link_libraries (augSesc ${EXTRA_LIBS})

