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
      bool isInterventionShared;
      bool isSpeculative;
		MessageID solicitingMessage;
		NodeID originalRequestingNode;

		// making directoryLookup false here is not enough,
		// because ReadResponseMsg can get reused from
		// Memsys/Pool::Take()
      ReadResponseMsg() : directoryLookup(false) {}
		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached ? size : 0); }
		virtual MsgType Type() const { return mt_ReadResponse; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
            << " satis=" << satisfied
		      << " blkAtt=" << blockAttached
            << " excluOwn=" << exclusiveOwnership
            << " solicMsg=" << solicitingMessage
            << " intShd=" << isInterventionShared
            << " spec=" << isSpeculative
		   ;
		}

	};
}
