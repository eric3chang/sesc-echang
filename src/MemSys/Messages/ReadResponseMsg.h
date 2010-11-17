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
      bool isWaitingForInvalidateUnlock;
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
            << " evi=" << evictionMessage
            << " exOw=" << exclusiveOwnership
            << " sMsg=" << solicitingMessage
            << " memAc=" << hasPendingMemAccesses
            //<< " isEvic=" << isFromEviction
            << " intv=" << isIntervention
            << " invUnlk=" << isWaitingForInvalidateUnlock
            << " ogNode=" << Memory::convertNodeIDToDeviceID(originalRequestingNode)
            << " pInv=" << pendingInvalidates
            //<< " spec=" << isSpeculative
		   ;
		}

	};
}
