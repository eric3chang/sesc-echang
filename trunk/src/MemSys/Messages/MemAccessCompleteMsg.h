#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class MemAccessCompleteMsg : public BaseMsg
	{
	public:
		Address addr;

		virtual bool IsResponse() const { return true; }
      virtual size_t MsgSize() const { return 1 + sizeof(Address);}
		virtual MsgType Type() const { return mt_MemAccessComplete; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " adr=" << addr
		   ;
		}
	};
}
