#pragma once
#include "AllMessageTypes.h"
#include "../MSTypes.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "BaseMsg.h"

using std::cout;
using std::endl;

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
		// debugging functions
		template <class Key>
		static void DumpMsgTemplate(HashMap<Key,const BaseMsg*> &m)
		{
			typename HashMap<Key,const BaseMsg*>::const_iterator i,j;
			i = m.begin();
			j = m.end();

			std::cout << "[" << "dump" << "]" << std::endl;
			for(; i != j; ++i)
			{
				std::cout << '[' << setw(2) << i->first << "]";
				i->second->print(0);
				std::cout << std::endl;
			}
		}

		BaseMemDevice();
		virtual ~BaseMemDevice();
		virtual void Initialize(EventManager* em, const RootConfigNode& config,
		   const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node) = 0;
		virtual void DumpStats(std::ostream& out) = 0;
		virtual void RecvMsg(const BaseMsg* msg, int connectionID) = 0;

		static Address GetAddress(const BaseMsg* m);

		int ConnectionCount();
		Connection& GetConnection(int index);
		EventManager& EM();
		// changed this because compiler sometimes think ID() is #define from nanassert
	   DeviceID GetDeviceID() const;
		const std::string& DeviceName();

      void printBaseMemDeviceDebugInfo(const char* childClass, const char* fromMethod,
            const BaseMsg &myBaseMsg, const char* operation, NodeID src)
      {
         if ((src == InvalidNodeID) || (src == 0))
         {
            cout << setw(8) << " ";
         }
         else
         {
            // convert from nodeID to deviceID
				cout << " src=" << setw(3) << BaseMsg::convertNodeIDToDeviceID(src);
            //cout << " src=" << setw(3) << (4*src)+7;
				//cout << " src=" << setw(3) << src;
         }
         myBaseMsg.print(deviceID);
         cout
            << " " << childClass << ":" << fromMethod
            << " " << operation
            << endl;
      }
      void printBaseMemDeviceDebugInfo(const char* childClass, const char* fromMethod,
            const BaseMsg &myBaseMsg, const char* operation)
      {
         printBaseMemDeviceDebugInfo(childClass,fromMethod,myBaseMsg,operation,
            InvalidNodeID);
      }
	};
} // namespace Memory
