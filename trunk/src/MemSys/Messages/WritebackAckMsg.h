#pragma once
#include "EvictionResponseMsg.h"

namespace Memory
{
	class WritebackAckMsg : public EvictionResponseMsg
	{
	public:
		bool isBusy;
		bool isExclusive;

		virtual MsgType Type() const { return mt_WritebackAck; }
		virtual void print(DeviceID destinationDeviceID) const
		{
			EvictionResponseMsg::print(destinationDeviceID);
			cout
				<< " isBusy=" << isBusy
				<< " isExclusive=" << isExclusive
			;
		}
	};
}

