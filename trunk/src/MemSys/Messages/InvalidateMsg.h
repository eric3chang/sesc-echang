#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class InvalidateMsg : public BaseMsg
	{
	public:
		Address addr;
      NodeID newOwner;
      MessageID solicitingMessage;
		size_t size;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address); }
		virtual MsgType Type() const { return mt_Invalidate; }
   protected:
      virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
            << " newOwner=" << convertDirectoryNetworkIDToDeviceNodeID(newOwner)
            << " sMsg=" << solicitingMessage
		   ;
		}
	};
}
