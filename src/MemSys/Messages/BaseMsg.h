#pragma once
#include "../MSTypes.h"
#include "../Debug.h"
#include <iostream>

namespace Memory
{
	class BaseMsg
	{
	private:
		MessageID msgID;
		DeviceID deviceID;
		Address generatingPC;

	public:
      // added virtual destructor to eliminate constructor warnings
      // Eric Chang 2010/07/30
		virtual ~BaseMsg(){}
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
			/*
			std::cout << "BaseMsg::SetIDInfo: " <<
			      "msgID=" << msgID <<
			      " devID=" << devID <<
			      " generatingPC=" << generatingPC << std::endl;
			      */
			std::cout << "BaseMsg::SetIDInnfo: msgID=" << msgID << std::endl;
			this->msgID = msgID;
			this->deviceID = devID;
			this->generatingPC = generatingPC;
		}
	};
}

