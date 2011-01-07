#pragma once
#include "ReadResponseMsg.h"

namespace Memory
{
	class ReadReplyMsg : public ReadResponseMsg
	{
	public:
      int pendingInvalidates;

		virtual MsgType Type() const { return mt_ReadReply; }
      virtual void print(DeviceID destinationDeviceID) const
		{
		   ReadResponseMsg::print(destinationDeviceID);
		   cout << " penInv=" << pendingInvalidates
		   ;
		}
	};
}

