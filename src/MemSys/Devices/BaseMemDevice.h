#pragma once
#include "../MSTypes.h"
#include <iostream>
#include <vector>
#include <string>

namespace Memory
{
	class Connection;
	class RootConfigNode;
	class EventManager;
	class BaseMsg;
	class BaseMemDevice
	{
		std::string deviceName;
		std::vector<Connection*> connectionSet;
		EventManager* em;
		DeviceID deviceID;
	public:
		BaseMemDevice();
		virtual ~BaseMemDevice();
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node) = 0;
		virtual void DumpStats(std::ostream& out) = 0;
		virtual void RecvMsg(const BaseMsg* msg, int connectionID) = 0;

		int ConnectionCount();
		Connection& GetConnection(int index);
		EventManager& EM();
		DeviceID ID() const;
		const std::string& DeviceName();
	};
}
