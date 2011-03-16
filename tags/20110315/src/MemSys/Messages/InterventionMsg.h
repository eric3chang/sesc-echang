#pragma once
#include "ReadMsg.h"

namespace Memory
{
	class InterventionMsg : public ReadMsg
	{
	public:
		MessageID solicitingMessage;
		NodeID newOwner;

		virtual bool IsResponse() const { return true; }
		virtual MsgType Type() const { return mt_Intervention; }
		virtual void print(DeviceID destinationDeviceID) const
		{
			ReadMsg::print(destinationDeviceID);
			cout << " sMsg=" << solicitingMessage
				<< " newOwner=" << BaseMsg::convertNodeIDToDeviceID(newOwner)
				;
		}
	};
}

