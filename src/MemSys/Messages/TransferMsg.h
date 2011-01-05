#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class TransferMsg : public EvictionMsg
	{
	public:
		bool isDirty;
		bool isShared;

		virtual MsgType Type() const { return mt_Transfer; }
      virtual void print(DeviceID destinationDeviceID) const
		{
		   EvictionMsg::print(destinationDeviceID);
		   cout << " dirty=" << isDirty
		         << " shared=" << isShared
		   ;
		}
	};
}
