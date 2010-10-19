#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class UnrequestedReadResponseMsg : public BaseMsg
	{
	public:
		Address addr;
      bool blockAttached; // should always be true
		size_t size;
      MessageID evictionMessage;
		bool exclusiveOwnership;
      // cannot have originalRequestingNode because it was unrequested
		//NodeID originalRequestingNode;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached ? size : 0); }
		virtual MsgType Type() const { return mt_UnrequestedReadResponse; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
            << " blkAtt=" << blockAttached
            << " evic=" << evictionMessage
            << " exOwn=" << exclusiveOwnership
            // cannot have originalRequestingNode because it was unrequested
            //<< " ogNode=" << convertDirectoryNetworkIDToDeviceNodeID(originalRequestingNode)
		   ;
		}

	};
}
