#pragma once
#include "BaseNakMsg.h"

namespace Memory
{
	class CacheNakMsg : public BaseNakMsg
	{
	public:
		MessageID interventionMessage;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_CacheNak; }
		virtual void print(DeviceID destinationDeviceID) const
		{
			BaseNakMsg::print(destinationDeviceID);
			cout << " intv=" << interventionMessage;
		}
	};
}

