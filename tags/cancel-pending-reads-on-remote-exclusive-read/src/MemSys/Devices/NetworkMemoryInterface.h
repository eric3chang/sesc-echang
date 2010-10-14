#pragma once
#include "BaseMemDevice.h"
#include "../MSTypes.h"
#include "../HashContainers.h"

namespace Memory
{
	class NetworkMemoryInterface : public BaseMemDevice
	{
		HashMap<MessageID, NodeID> sourceTable;
		NodeID nodeID;
		int memoryConnectionID;
		int networkConnectionID;
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
