#pragma once
#include "BaseMemDevice.h"
#include "../MSTypes.h"

namespace Memory
{
	class TestMemory : public BaseMemDevice
	{
		TimeDelta writeTime;
		TimeDelta readTime;
		unsigned long long evictionsReceived;
		unsigned long long readsReceived;
		unsigned long long writesReceived;
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
