#pragma once
#include "Factory.h"
#include "MSTypes.h"
#include <vector>
#include <string>
#include <iostream>

namespace Memory
{
	class BaseMemDevice;
	class EventManager;
	class ConfigNode;
	class Connection;
	class MemorySystem
	{
		class ConnectionDescription
		{
		public:
			std::string deviceA;
			std::string deviceB;
			std::string nameA;
			std::string nameB;
			TimeDelta latency;
		};
		std::vector<Connection*> connectionSet;
		std::vector<BaseMemDevice*> deviceSet;
		std::vector<EventManager*> eventManagerSet;
		HashMap<std::string,BaseMemDevice*> interfaceDevices;
		Factory<std::string,BaseMemDevice> deviceFactory;
		int DeviceID(const std::string& name);
		bool ExistsDevice(const std::string& name);
	public:
		MemorySystem();
		~MemorySystem();

		void Initialize(const std::string inputFile);
		void Initialize(std::istream& inStream);
		void Initialize(const ConfigNode& config);

		void SubmitInterfaceDevice(const std::string& name,BaseMemDevice* device);

		void ProcessTick();
	};
}

