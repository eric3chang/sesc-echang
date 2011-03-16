/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau
                  Milos Prvulovic
This file is part of SESC.

SESC is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2, or (at your option) any later version.

SESC is    distributed in the  hope that  it will  be  useful, but  WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of  the GNU General  Public License along with
SESC; see the file COPYING.  If not, write to the  Free Software Foundation, 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef SNIPPETS_H
#define SNIPPETS_H

#include <stdint.h>
#if !(defined MIPS_EMUL)
#include "mendian.h"
#endif

//**************************************************
// Generic typedefs
typedef uint8_t        uchar;
typedef uint16_t       ushort;
typedef unsigned long  ulong;
typedef uint64_t ulonglong;

//**************************************************
// Process typedefs

// CPU_t is signed because -1 means that it is not mapped to any CPU.
typedef int32_t CPU_t;

// Only the lower 16 bits are valid (at most 64K threads), but
// negative values may have special meaning (invalid == -1)
typedef int32_t         Pid_t;

//**************************************************
// Types used for time (move to callback?)
typedef uint64_t Time_t;
const uint64_t MaxTime = ((~0ULL) - 1024);  // -1024 is to give a little bit of margin

extern Time_t globalClock; // Defined in Thread.cpp

typedef uint32_t TimeDelta_t;
const uint32_t MaxDeltaTime = (65535 - 1024);  // -1024 is to give a little bit of margin

//**************************************************
// Memory subsystem
typedef intptr_t Address;

short  log2i(uint32_t n);

//x, y are integers and x,y > 0
#define CEILDiv(x,y)            ((x)-1)/(y)+1

uint32_t roundUpPower2(uint32_t x);

void debugAccess();

#define K(n) ((n) * 1024)
#define M(n) (K(n) * 1024)
#define G(n) (M(n) * 1024)

#endif // SNIPPETS_H
