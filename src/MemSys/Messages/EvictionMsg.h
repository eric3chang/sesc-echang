#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class EvictionMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool blockAttached;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached?size:0); }
		virtual MsgType Type() const { return mt_Eviction; }
		virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " adr=" << addr
		         << " blockAttached=" << blockAttached
		   ;
		}
	};
}
