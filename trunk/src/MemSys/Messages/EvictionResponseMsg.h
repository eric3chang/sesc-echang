#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class EvictionResponseMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		MessageID solicitingMessage;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address); }
		virtual MsgType Type() const { return mt_EvictionResponse; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
      {
         BaseMsg::print(destinationDeviceID);
         cout << " addr=" << addr
               << " solicitingMessage=" << solicitingMessage
         ;
      }
	};
}
