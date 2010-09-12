#pragma once

// added this to remove warnings in eclipse
// when using unsigned int and unsigned long long
#include <sys/types.h>

namespace Memory
{
	typedef unsigned int Address;
	typedef unsigned int ProcessorID;
	typedef unsigned int DeviceID;
	typedef int NodeID;
	const NodeID InvalidNodeID = -1;
	typedef unsigned long long MessageID;
	typedef unsigned long long TickTime;
	typedef unsigned long long TimeDelta;
	enum MsgType
	{
		mt_Read,
		mt_Write,
		mt_Invalidate,
		mt_Eviction,
		mt_InvalidateSharer,

		mt_ReadResponse,
		mt_WriteResponse,
		mt_InvalidateResponse,
		mt_EvictionResponse,

		mt_Network,
	};
}
