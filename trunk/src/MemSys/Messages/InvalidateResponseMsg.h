#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class InvalidateResponseMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool blockAttached;
		MessageID solicitingMessage;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached?size:0); }
		virtual MsgType Type() const { return mt_InvalidateResponse; }
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " adr=" << addr
		         << " blockAttached=" << blockAttached
               << " solicitingMessage=" << solicitingMessage
		   ;
		}
	};
}
