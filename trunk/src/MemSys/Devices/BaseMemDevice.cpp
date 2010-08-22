#include "BaseMemDevice.h"
#include "../Connection.h"
#include "../Configuration.h"
#include "../EventManager.h"

// toggle debug messages for this class
//#define MEMORY_BASE_MEM_DEVICE_DEBUG

namespace Memory
{
	void BaseMemDevice::SendMsg(int connectionID, const BaseMsg* msg, TimeDelta delay)
	{
		DebugAssert(msg);
		GetConnection(connectionID).SendMsg(msg,delay);
   }
	BaseMemDevice::BaseMemDevice(){}
	BaseMemDevice::~BaseMemDevice(){}
	void BaseMemDevice::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		DebugAssert(em);
		DebugAssert(connectionSet.size() > 0);
		for(size_t i = 0; i < connectionSet.size(); i++)
		{
			this->connectionSet.push_back(connectionSet[i]);
		}
		this->em = em;
		DebugAssert(config.ContainsSubNode("DeviceName"));
		DebugAssert(config.SubNode("DeviceName").IsA<StringConfigNode>());
		const StringConfigNode& devNameNode = (const StringConfigNode&)config.SubNode("DeviceName");
		deviceName = devNameNode.Value();
		DebugAssert(config.ContainsSubNode("DeviceID"));
		DebugAssert(config.SubNode("DeviceID").IsA<IntConfigNode>());
		const IntConfigNode& devIDNode = (const IntConfigNode&)config.SubNode("DeviceID");
		deviceID = (DeviceID)devIDNode.Value();
	}
	int BaseMemDevice::ConnectionCount()
	{
		return (int)connectionSet.size();
	}
	Connection& BaseMemDevice::GetConnection(int index)
	{
		DebugAssert(index >= 0 && index < (int)connectionSet.size());

#ifdef MEMORY_BASE_MEM_DEVICE_DEBUG
		std::cout << "BaseMemDevice::GetConnection: " << std::endl;
		for (int i=0; i<(int)connectionSet.size(); i++)
		{
		   std::cout << "   ";
		   connectionSet[i]->print();
		}
#endif
		return *(connectionSet[index]);
	}
	EventManager& BaseMemDevice::EM()
	{
		return *em;
	}
	DeviceID BaseMemDevice::ID() const
	{
	   return deviceID;
	}
	DeviceID BaseMemDevice::getDeviceID() const
	{
		return ID();
	}
	const std::string& BaseMemDevice::DeviceName()
	{
		return deviceName;
	}
}
