#pragma once
#include "../MSTypes.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "BaseMsg.h"

/*
// allows one to get the variable name of n
#ifndef GET_NAME
   #define GET_NAME(n) #n
#endif
*/

namespace Memory
{
	class Connection;
	class RootConfigNode;
	class EventManager;
	//class BaseMsg;
	class BaseMemDevice
	{
		std::string deviceName;
		std::vector<Connection*> connectionSet;
		EventManager* em;
		DeviceID deviceID;
	protected:
		void SendMsg(int connectionID, const BaseMsg* msg, TimeDelta delay);
		typedef PooledFunctionGenerator<StoredClassFunction3<BaseMemDevice,
         int, const BaseMsg*, TimeDelta, &Memory::BaseMemDevice::SendMsg> >
         CBSendMsg;
		CBSendMsg cbSendMsg;

	public:
		BaseMemDevice();
		virtual ~BaseMemDevice();
		virtual void Initialize(EventManager* em, const RootConfigNode& config,
		   const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node) = 0;
		virtual void DumpStats(std::ostream& out) = 0;
		virtual void RecvMsg(const BaseMsg* msg, int connectionID) = 0;

		int ConnectionCount();
		Connection& GetConnection(int index);
		EventManager& EM();
		DeviceID ID() const;
		// added this because compiler sometimes think ID() is #define from nanassert
	   DeviceID getDeviceID() const;
		const std::string& DeviceName();

	   void printBaseMemDeviceDebugInfo(const char* childClass,
	         const char* fromMethod,MessageID myMessageID, const char* operation)
	   {
	      std::cout
            << "devID=" << std::setw(2) << deviceID
            << ": msgID=" << std::setw(6) << (MessageID) myMessageID
            << " " << childClass << "::" << fromMethod
	         << " " << operation
	      << std::endl;
	   }

      void printBaseMemDeviceDebugInfo(const char* childClass,
            const char* fromMethod, const BaseMsg &myBaseMsg, const char* operation)
      {
         myBaseMsg.print(deviceID);
         std::cout
            << " " << childClass << "::" << fromMethod
            << " " << operation
         << std::endl;
      }
	};
} // namespace Memory
