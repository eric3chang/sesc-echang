#pragma once
#include <assert.h>

inline void DebugFaultCatcher()
{
	int a = 0;//this doesn't do anything special
   //it is just a spot to drop break points that is convenient
	a++;	// to stop the compiler from complaining about unused variable
}

#if defined(DEBUG) || defined(_DEBUG)
   #define DebugAssertWithMessageID(x,mid) if(!(x)){ \
      std::cout << "Failed at MessageID=" << mid << std::endl; DebugFaultCatcher(); assert(x);}
   #define DebugAssert(x) if(!(x)){ DebugFaultCatcher(); assert(x);}
   #define DebugFail(x) if(true){ DebugFaultCatcher(); assert(x == 0); assert(x);}
#else
   #define DebugAssertWithMessageID(x,mid)
   #define DebugAssert(x)
   #define DebugFail(x) assert(x == 0)
#endif
