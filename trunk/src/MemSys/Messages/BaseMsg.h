#pragma once
#include "../MSTypes.h"
#include "../Debug.h"
#include <iostream>
#include <iomanip>

// toggle debug messages
//#define MEMORY_BASEMSG_DEBUG

using std::cout;
using std::endl;
using std::setw;

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

		virtual void print() const
		{
		   cout
		      << " msgID=" << setw(6) << msgID
		      << " devID=" << setw(2) << deviceID
		      << " generatingPC=" << generatingPC
		   ;
		}

		void SetIDInfo(MessageID msgID, DeviceID devID, Address generatingPC)
		{
			DebugAssert(msgID);
#ifdef MEMORY_BASEMSG_DEBUG
			std::cout << "BaseMsg::SetIDInfo: msgID=" << msgID
			      << " devID=" << devID
			      <<" generatingPC=" << generatingPC << std::endl;
#endif
			this->msgID = msgID;
			this->deviceID = devID;
			this->generatingPC = generatingPC;
		}
	};
}

