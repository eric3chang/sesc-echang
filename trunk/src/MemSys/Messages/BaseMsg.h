#pragma once
#include "../MSTypes.h"
#include "../Debug.h"

namespace Memory
{
	class BaseMsg
	{
		MessageID msgID;
		DeviceID deviceID;
		Address generatingPC;

	public:
		virtual bool IsResponse() const = 0;
		virtual size_t MsgSize() const = 0;
		virtual MsgType Type() const = 0;

		Address GeneratingPC() const
		{
			return generatingPC;
		}
		bool GeneratingPCValid() const
		{
			return (generatingPC != 0);
		}
		MessageID MsgID() const
		{
			return msgID;
		}
		DeviceID GeneratingDeviceID() const
		{
			return deviceID;
		}

		void SetIDInfo(MessageID msgID, DeviceID devID, Address generatingPC)
		{
			DebugAssert(msgID);
			this->msgID = msgID;
			this->deviceID = devID;
			this->generatingPC = generatingPC;
		}
	};
}

