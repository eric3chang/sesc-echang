add_library (libpint
   PPCDecoder.cpp
)

set (EXTRA_LIBS ${EXTRA_LIBS} libpint)
