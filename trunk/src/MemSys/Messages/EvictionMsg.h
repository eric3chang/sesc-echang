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
      bool isBlockNotFound;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached?size:0); }
		virtual MsgType Type() const { return mt_Eviction; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
		         << " blockAttached=" << blockAttached
               << " blkNotFnd=" << isBlockNotFound
		   ;
		}
	};
}
