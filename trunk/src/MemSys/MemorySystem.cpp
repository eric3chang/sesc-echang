#include "MemorySystem.h"
#include "Devices/AllDeviceTypes.h"
#include "EventManager.h"
#include "Configuration.h"
#include "Connection.h"
#include "Debug.h"
#include <fstream>

namespace Memory
{
	int MemorySystem::DeviceID(const std::string& name)
	{
		for(size_t i = 0; i < deviceSet.size(); i++)
		{
			if(deviceSet[i]->DeviceName() == name)
			{
				return (int)i;
			}
		}
		DebugFail("No device with name");
		return -1;
	}
	bool MemorySystem::ExistsDevice(const std::string& name)
	{
		for(size_t i = 0; i < deviceSet.size(); i++)
		{
			if(deviceSet[i]->DeviceName() == name)
			{
				return true;
			}
		}
		return false;
	}
	MemorySystem::MemorySystem()
	{
	   deviceFactory.RegisterNode("Directory",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<Directory>());
	   deviceFactory.RegisterNode("ThreeStageDirectory",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<ThreeStageDirectory>());
		deviceFactory.RegisterNode("MOESICache",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<MOESICache>());
		deviceFactory.RegisterNode("NetworkMemoryInterface",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<NetworkMemoryInterface>());
		deviceFactory.RegisterNode("RandomLoadNetwork",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<RandomLoadNetwork>());
		deviceFactory.RegisterNode("SnoopyBus",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<SnoopyBus>());
		deviceFactory.RegisterNode("TestMemory",new Factory<std::string,BaseMemDevice>::GenericFactoryNode<TestMemory>());
	}
	MemorySystem::~MemorySystem()
	{
		for(size_t i = 0; i < deviceSet.size(); i++)
		{
			delete deviceSet[i];
		}
		for(size_t i = 0; i < eventManagerSet.size(); i++)
		{
			delete eventManagerSet[i];
		}
		for(size_t i = 0; i < connectionSet.size(); i++)
		{
			delete connectionSet[i];
		}
	}
	void MemorySystem::Initialize(const std::string inputFile)
	{
		std::ifstream in(inputFile.c_str(),std::ios::in);
		Initialize(in);
		in.close();
	}
	void MemorySystem::Initialize(std::istream& inStream)
	{
		RootConfigNode* root = new RootConfigNode("Root");
		root->LoadFrom(inStream);
		Initialize(*root);
	}
	void MemorySystem::Initialize(const Memory::ConfigNode& config)
	{
		DebugAssert(config.IsA<RootConfigNode>());
		const RootConfigNode& rc = (const RootConfigNode&)config;
		int deviceCount = rc.SubNodeCount("MemoryDevice");
		int connectionCount = rc.SubNodeCount("Connection");
		DebugAssert(deviceCount > 0);
		DebugAssert(connectionCount > 0);
		std::vector<std::string> deviceNameSet;
		for(int i = 0; i < deviceCount; i++)
		{
			DebugAssert(rc.SubNode("MemoryDevice",i).IsA<RootConfigNode>());
			const RootConfigNode& devConfig = ((const RootConfigNode&)rc.SubNode("MemoryDevice",i));
			DebugAssert(devConfig.ContainsSubNode("DeviceType") && devConfig.SubNode("DeviceType").IsA<StringConfigNode>());
			DebugAssert(devConfig.ContainsSubNode("DeviceName") && devConfig.SubNode("DeviceName").IsA<StringConfigNode>());
			if(interfaceDevices.find(((const StringConfigNode&)(devConfig.SubNode("DeviceName"))).Value()) != interfaceDevices.end())
			{
				deviceSet.push_back(interfaceDevices[((const StringConfigNode&)(devConfig.SubNode("DeviceName"))).Value()]);
				interfaceDevices.erase(((const StringConfigNode&)(devConfig.SubNode("DeviceName"))).Value());
			}
			else
			{
			   //std::cout << "MemorySystem::Initialize: " << ((const StringConfigNode&)(devConfig.SubNode("DeviceType"))).Value() << std::endl;
				BaseMemDevice* d = deviceFactory.Create(((const StringConfigNode&)(devConfig.SubNode("DeviceType"))).Value());
				deviceSet.push_back(d);
			}
			deviceNameSet.push_back(((const StringConfigNode&)(devConfig.SubNode("DeviceName"))).Value());
		}
		std::vector<ConnectionDescription> conDescs;
		for(int i = 0; i < connectionCount; i++)
		{
			ConnectionDescription c;
			DebugAssert(rc.SubNode("Connection",i).IsA<RootConfigNode>());
			const RootConfigNode& conConfig = ((const RootConfigNode&)rc.SubNode("Connection",i));
			DebugAssert(conConfig.ContainsSubNode("FromDevice") && conConfig.SubNode("FromDevice").IsA<StringConfigNode>());
			DebugAssert(conConfig.ContainsSubNode("ToDevice") && conConfig.SubNode("ToDevice").IsA<StringConfigNode>());
			DebugAssert(conConfig.ContainsSubNode("FromName") && conConfig.SubNode("FromName").IsA<StringConfigNode>());
			DebugAssert(conConfig.ContainsSubNode("ToName") && conConfig.SubNode("ToName").IsA<StringConfigNode>());
			DebugAssert(conConfig.ContainsSubNode("Latency") && conConfig.SubNode("Latency").IsA<IntConfigNode>());
			c.deviceA = ((const StringConfigNode&)conConfig.SubNode("FromDevice")).Value();
			c.deviceB = ((const StringConfigNode&)conConfig.SubNode("ToDevice")).Value();
			c.nameA = ((const StringConfigNode&)conConfig.SubNode("FromName")).Value();
			c.nameB = ((const StringConfigNode&)conConfig.SubNode("ToName")).Value();
			c.latency = (TimeDelta)((const IntConfigNode&)conConfig.SubNode("Latency")).Value();
			conDescs.push_back(c);
		}
		std::vector<int> connectionSeen;
		std::vector<std::vector<Connection*> > tempConSet;
		for(size_t i = 0; i < deviceSet.size(); i++)
		{
			connectionSeen.push_back(0);
			tempConSet.push_back(std::vector<Connection*>());
		}
		//change in parallel version
		eventManagerSet.push_back(new EventManager(1,0));
		for(size_t i = 0; i < conDescs.size(); i++)
		{
			int fromDeviceID = -1;
			int toDeviceID = -1;
			for(size_t j = 0; j < deviceNameSet.size(); j++)
			{
				if(deviceNameSet[j] == conDescs[i].deviceA)
				{
					fromDeviceID = (int)j;
				}
				if(deviceNameSet[j] == conDescs[i].deviceB)
				{
					toDeviceID = (int)j;
				}
			}
			DebugAssert(fromDeviceID != -1);
			DebugAssert(toDeviceID != -1);
			DebugAssert(fromDeviceID != toDeviceID);
			tempConSet[fromDeviceID].push_back(new Connection(conDescs[i].nameA,deviceSet[fromDeviceID],deviceSet[toDeviceID],connectionSeen[toDeviceID],conDescs[i].latency,eventManagerSet[0]));
			tempConSet[toDeviceID].push_back(new Connection(conDescs[i].nameB,deviceSet[toDeviceID],deviceSet[fromDeviceID],connectionSeen[fromDeviceID],conDescs[i].latency,eventManagerSet[0]));
			connectionSeen[fromDeviceID]++;
			connectionSeen[toDeviceID]++;
		}
		for(size_t i = 0; i < deviceSet.size(); i++)
		{
			deviceSet[i]->Initialize(eventManagerSet[0],(const RootConfigNode&)rc.SubNode("MemoryDevice",(int)i),tempConSet[i]);
		}
		//end change in parallel version
		for(size_t i = 0; i < tempConSet.size(); i++)
		{
			for(size_t j = 0; j < tempConSet[i].size(); j++)
			{
				connectionSet.push_back(tempConSet[i][j]);
			}
		}
		// check that there's the number of interfaces specified .memory is >= than in .conf
		DebugAssert(interfaceDevices.empty());
	}
	void MemorySystem::SubmitInterfaceDevice(const std::string& name, BaseMemDevice* device)
	{
		DebugAssert(device);
		DebugAssert(name != "");
		DebugAssert(interfaceDevices.find(name) == interfaceDevices.end());
		interfaceDevices[name] = device;
	}
	void MemorySystem::ProcessTick()
	{
		for(size_t i = 0; i < eventManagerSet.size();i++)
		{
			eventManagerSet[i]->ProcessTick();
		}
	}
}
