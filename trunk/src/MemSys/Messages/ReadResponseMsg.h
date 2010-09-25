#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
   const int INVALID_PENDING_INVALIDATES = -2;

	class ReadResponseMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool satisfied;
		bool blockAttached;
      bool directoryLookup;
		bool exclusiveOwnership;
      bool isIntervention;
      int pendingInvalidates;
      //bool isSpeculative;   // not implementing speculative right now
		MessageID solicitingMessage;
		NodeID originalRequestingNode;

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
            << " interv=" << isIntervention
            << " pendInv=" << pendingInvalidates
            //<< " spec=" << isSpeculative
		   ;
		}

	};
}
