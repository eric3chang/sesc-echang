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
		bool isCache;
		bool isDirectory;
		virtual ~BaseMsg(){}
		virtual bool IsResponse() const = 0;
		virtual size_t MsgSize() const = 0;
		virtual MsgType Type() const = 0;

	   /**
	   converts networkNodeID (0,1,2...) to deviceNodeID (7,11,15...)
	   */
	   static int convertNodeIDToDeviceID(NodeID myNodeID)
	   {
	      return(myNodeID*5 + 5);
	   }

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
		virtual void print(DeviceID destinationDeviceID) const
		{
         cout
         //cout << std::hex
            << " mSrc=" << setw(3) << deviceID
		      << " dst=" << setw(3) << destinationDeviceID
		      << " msg=" << setw(6) << msgID
		      // generatingPC doesn't seem to be used
		      //<< " generatingPC=" << generatingPC
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

