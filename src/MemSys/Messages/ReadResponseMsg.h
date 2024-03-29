#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class ReadResponseMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool satisfied;
		bool blockAttached;
      bool directoryLookup;
		bool exclusiveOwnership;
		MessageID interventionMessage;
		bool isDirty;
		MessageID solicitingMessage;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached ? size : 0); }
		virtual MsgType Type() const { return mt_ReadResponse; }
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " adr=" << addr
            <<" sat=" << satisfied
		      << " at=" << blockAttached
            << " exOw=" << exclusiveOwnership
            << " intv=" << interventionMessage
            << " dirty=" << isDirty
            << " sMsg=" << solicitingMessage
		   ;
		}

	};
}
