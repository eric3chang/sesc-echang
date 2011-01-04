add_executable (augSesc "${PROJECT_SOURCE_DIR}/libmem/mtst1.cpp")
target_link_libraries (augSesc core ll mem mint pint suc trans MemSys Devices)
