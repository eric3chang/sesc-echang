#pragma once
#include <string>
#include "MSTypes.h"

namespace Memory
{
	class BaseMemDevice;
	class EventManager;
	class BaseMsg;
	class Connection
	{
		std::string name;
		BaseMemDevice* from;
		BaseMemDevice* to;
		EventManager* ev;
		TimeDelta latency;
		int toConnectionID;
	public:
		Connection(std::string linkName, BaseMemDevice* from, BaseMemDevice* to, int toConnectionID, TimeDelta delay, EventManager* ev);
		void SendMsg(const BaseMsg* msg, TimeDelta delay = 0);
		const std::string& Name();
	};
}
