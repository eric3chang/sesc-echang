#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class InvalidateSharerMsg : public BaseMsg
	{
	public:
		Address addr;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address); }
		virtual MsgType Type() const { return mt_InvalidateSharer; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
		   ;
		}
	};
}
