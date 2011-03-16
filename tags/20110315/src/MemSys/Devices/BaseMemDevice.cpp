#include "BaseMemDevice.h"
#include "../Connection.h"
#include "../Configuration.h"
#include "../EventManager.h"

// toggle debug messages for this class
//#define MEMORY_BASE_MEM_DEVICE_DEBUG_COMMON
//#define MEMORY_BASE_MEM_DEVICE_DEBUG_VERBOSE

/*
// allows one to get the variable name of n
#ifndef GET_NAME
   #define GET_NAME(n) #n
#endif
*/

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

#ifdef MEMORY_BASE_MEM_DEVICE_DEBUG_VERBOSE
		std::cout << "BaseMemDevice::GetConnection: " << std::endl;
		for (int i=0; i<(int)connectionSet.size(); i++)
		{
		   std::cout << "   ";
		   connectionSet[i]->print();
		}
#endif
#if defined MEMORY_BASE_MEM_DEVICE_DEBUG_COMMON && !defined _WIN32
   #define MEMORY_BASE_MEM_DEVICE_DEBUG_ARRAY_SIZE 200
		Connection** connectionArray;
		connectionArray = (Connection**)malloc(sizeof(Connection*) *
		      MEMORY_BASE_MEM_DEVICE_DEBUG_ARRAY_SIZE);
		convertVectorToArray<Connection*>(connectionSet, connectionArray,
		      MEMORY_BASE_MEM_DEVICE_DEBUG_ARRAY_SIZE);
		free(connectionArray);
#endif
		return *(connectionSet[index]);
	}
	Address BaseMemDevice::GetAddress(const BaseMsg* m)
	{
		switch(m->Type())
		{
		case (mt_Eviction):
			return ((const EvictionMsg*)m)->addr;
		case (mt_EvictionResponse):
			return ((const EvictionResponseMsg*)m)->addr;
		case (mt_Invalidate):
			return ((const InvalidateMsg*)m)->addr;
		case (mt_InvalidateResponse):
			return ((const InvalidateResponseMsg*)m)->addr;
		case (mt_Network):
			return 0;
		case (mt_Read):
			return ((const ReadMsg*)m)->addr;
		case (mt_ReadResponse):
			return ((const ReadResponseMsg*)m)->addr;
		case (mt_Write):
			return ((const WriteMsg*)m)->addr;
		case (mt_WriteResponse):
			return ((const WriteResponseMsg*)m)->addr;

		case (mt_CacheNak):
			return ((const CacheNakMsg*)m)->addr;
		case (mt_DirectoryNak):
			return ((const DirectoryNakMsg*)m)->addr;
		case (mt_Intervention):
			return ((const InterventionMsg*)m)->addr;
		case (mt_InvalidateAck):
			return ((const InvalidateAckMsg*)m)->addr;
		case (mt_ReadReply):
			return ((const ReadReplyMsg*)m)->addr;
		case (mt_SpeculativeReply):
			return ((const SpeculativeReplyMsg*)m)->addr;
		case (mt_Transfer):
			return ((const TransferMsg*)m)->addr;
		case (mt_Writeback):
			return ((const WritebackMsg*)m)->addr;
		case (mt_WritebackAck):
			return ((const WritebackAckMsg*)m)->addr;
		case (mt_WritebackRequest):
			return ((const WritebackRequestMsg*)m)->addr;
		}
		return 0;
	}
	EventManager& BaseMemDevice::EM()
	{
		return *em;
	}
	DeviceID BaseMemDevice::GetDeviceID() const
	{
		return deviceID;
	}
	const std::string& BaseMemDevice::DeviceName()
	{
		return deviceName;
	}
}
