#pragma once

namespace Memory
{
	typedef unsigned int Address;
	typedef unsigned int ProcessorID;
	typedef unsigned int DeviceID;
	typedef unsigned long long MessageID;
	typedef unsigned long long TickTime;
	typedef unsigned long long TimeDelta;
	enum MsgType
	{
		mt_Read,
		mt_Write,
		mt_Invalidate,
		mt_Eviction,

		mt_ReadResponse,
		mt_WriteResponse,
		mt_InvalidateResponse,
		mt_EvictionResponse,
	};
}