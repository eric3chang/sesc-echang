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
#include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/libapp")
include_directories ("${PROJECT_SOURCE_DIR}/libcore")
include_directories ("${PROJECT_SOURCE_DIR}/libemul")
include_directories ("${PROJECT_SOURCE_DIR}/libll")
include_directories ("${PROJECT_SOURCE_DIR}/libmem")
include_directories ("${PROJECT_SOURCE_DIR}/libmint")
include_directories ("${PROJECT_SOURCE_DIR}/libnet")
include_directories ("${PROJECT_SOURCE_DIR}/libpint")
include_directories ("${PROJECT_SOURCE_DIR}/libpower")
include_directories ("${PROJECT_SOURCE_DIR}/librst")
include_directories ("${PROJECT_SOURCE_DIR}/libsescspot")
include_directories ("${PROJECT_SOURCE_DIR}/libsmp")
include_directories ("${PROJECT_SOURCE_DIR}/libsuc")
include_directories ("${PROJECT_SOURCE_DIR}/libsuperlu")
include_directories ("${PROJECT_SOURCE_DIR}/libtm")
include_directories ("${PROJECT_SOURCE_DIR}/libtrans")
include_directories ("${PROJECT_SOURCE_DIR}/libvmem")
include_directories ("${PROJECT_SOURCE_DIR}/MemSys")

# add compilation of the necessary directories of sesc
add_subdirectory (libcore)

#  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)

# add the executable
#add_executable (augSesc tutorial.cxx)
#target_link_libraries (Tutorial  ${EXTRA_LIBS})

