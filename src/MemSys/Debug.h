#pragma once
#include <assert.h>

inline void DebugFaultCatcher()
{
	int a = 0;//this doesn't do anything special
	//it is just a spot to drop break points that is convenient
}

#if defined(DEBUG) || defined(_DEBUG)
#define DebugAssert(x) if(!(x)){ DebugFaultCatcher(); assert(x);}
#define DebugFail(x) if(true){ DebugFaultCatcher(); assert(x == 0); assert(x);}
#else
#define DebugAssert(x) 
#define DebugFail(x) assert(x == 0)
#endif

