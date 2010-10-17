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
      MessageID evictionMessage;
		bool exclusiveOwnership;
      bool hasPendingMemAccesses;
      //bool isFromEviction;
      bool isIntervention;
      bool isWaitingForInvalidateUnblock;
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
            <<" sat=" << satisfied
		      << " att=" << blockAttached
            << " evic=" << evictionMessage
            << " exOwn=" << exclusiveOwnership
            << " sMsg=" << solicitingMessage
            << " memAcc=" << hasPendingMemAccesses
            //<< " isEvic=" << isFromEviction
            << " intv=" << isIntervention
            << " invUnblk=" << isWaitingForInvalidateUnblock
            << " ogNode=" << convertDirectoryNetworkIDToDeviceNodeID(originalRequestingNode)
            << " pendInv=" << pendingInvalidates
            //<< " spec=" << isSpeculative
		   ;
		}

	};
}
