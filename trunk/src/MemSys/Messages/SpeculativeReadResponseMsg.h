#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class SpeculativeReadResponseMsg : public BaseMsg
	{
	public:
		Address addr;
		MessageID solicitingMessage;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address); }
		virtual MsgType Type() const { return mt_SpeculativeReadResponse; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
            << " solicMsg=" << solicitingMessage
		   ;
		}

	};
}
