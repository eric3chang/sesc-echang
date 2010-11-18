#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class EvictionBusyAckMsg : public BaseMsg
	{
	public:
		Address addr;
      bool isExclusive;
		size_t size;
		MessageID solicitingMessage;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address); }
		virtual MsgType Type() const { return mt_EvictionBusyAck; }
      virtual void print(DeviceID destinationDeviceID) const
      {
         BaseMsg::print(destinationDeviceID);
         cout << " adr=" << addr
            << " isExclusive=" << isExclusive
            << " solicitMsg=" << solicitingMessage
         ;
      }
	};
}
