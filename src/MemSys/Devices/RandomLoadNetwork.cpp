#include "RandomLoadNetwork.h"
#include "../Configuration.h"
#include "../Messages/NetworkMsg.h"
#include "../Debug.h"
#include "../EventManager.h"
#include "../Connection.h"
#include <cstdlib>
#include <vector>
#include <string>

namespace Memory
{
	TimeDelta RandomLoadNetwork::ConstantCalculator::CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{
		return timeTaken + msg->MsgSize() * perByte;
	}
	void RandomLoadNetwork::ConstantCalculator::OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{}
	void RandomLoadNetwork::ConstantCalculator::OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{}
	void RandomLoadNetwork::ConstantCalculator::Initialize(const RootConfigNode& config, int nodeCount)
	{
		timeTaken = (TimeDelta)Config::GetInt(config, "TimeTaken");
		perByte = (TimeDelta)Config::GetInt(config, "TimePerByte");
	}
	TimeDelta RandomLoadNetwork::RandomCalculator::CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{
		TimeDelta dtBase = initialTime + (rand() % randomRange);
		TimeDelta dtPacket = perByte * msg->MsgSize();
		if(enforceInOrder)
		{
			if(mostRecentMsg[fromNode][toNode] > time + dtBase)
			{
				dtBase = mostRecentMsg[fromNode][toNode] - (time + dtBase);
			}
			mostRecentMsg[fromNode][toNode] = time + dtBase + dtPacket;
		}
		return  dtBase + dtPacket;
	}
	void RandomLoadNetwork::RandomCalculator::OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{}
	void RandomLoadNetwork::RandomCalculator::OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{}
	void RandomLoadNetwork::RandomCalculator::Initialize(const RootConfigNode& config, int nodeCount)
	{
		TimeDelta minTime = (TimeDelta)Config::GetInt(config, "MinTime");
		TimeDelta maxTime = (TimeDelta)Config::GetInt(config, "MaxTime");
		DebugAssert(minTime < maxTime);
		initialTime = minTime;
		randomRange = (maxTime - minTime) + 1;
		enforceInOrder = (bool)Config::GetIntOrElse(1, config, "EnforceOrder");
		if(enforceInOrder)
		{
			for(int i = 0; i < nodeCount; i++)
			{
				mostRecentMsg.push_back(std::vector<TickTime>());
				for(int j = 0; j < nodeCount; j++)
				{
					mostRecentMsg[i].push_back(0);
				}
			}
		}
		perByte = (TimeDelta)Config::GetInt(config, "TimePerByte");
	}
	TimeDelta RandomLoadNetwork::RandomLoadedCalculator::CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{
		TimeDelta dtBase = initialTime + (rand() % randomRange) + (TimeDelta)((float)packetsInTransit * timePerPacket);
		TimeDelta dtPacket = perByte * msg->MsgSize();
		if(enforceInOrder)
		{
			if(mostRecentMsg[fromNode][toNode] > time + dtBase)
			{
				dtBase = mostRecentMsg[fromNode][toNode] - (time + dtBase);
			}
			mostRecentMsg[fromNode][toNode] = time + dtBase + dtPacket;
		}
		return dtBase + dtPacket;
	}
	void RandomLoadNetwork::RandomLoadedCalculator::OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{
		packetsInTransit++;
		DebugAssert(packetsInTransit > 0);
	}
	void RandomLoadNetwork::RandomLoadedCalculator::OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time)
	{
		DebugAssert(packetsInTransit > 0);
		packetsInTransit--;
	}
	void RandomLoadNetwork::RandomLoadedCalculator::Initialize(const RootConfigNode& config, int nodeCount)
	{
		TimeDelta minTime = (TimeDelta)Config::GetInt(config, "MinTime");
		TimeDelta maxTime = (TimeDelta)Config::GetInt(config, "MaxTime");
		DebugAssert(minTime < maxTime);
		initialTime = minTime;
		randomRange = (maxTime - minTime) + 1;
		enforceInOrder = (bool)Config::GetIntOrElse(1, config, "EnforceOrder");
		if(enforceInOrder)
		{
			for(int i = 0; i < nodeCount; i++)
			{
				mostRecentMsg.push_back(std::vector<TickTime>());
				for(int j = 0; j < nodeCount; j++)
				{
					mostRecentMsg[i].push_back(0);
				}
			}
		}
		perByte = (TimeDelta)Config::GetInt(config, "TimePerByte");
		timePerPacket = (float)Config::GetReal(config, "TimePerPacket");
		packetsInTransit = 0;
	}
	void RandomLoadNetwork::DeliverMsg(const NetworkMsg* m)
	{
		DebugAssert(m);
		DebugAssert(nodeToConnection.find(m->sourceNode) != nodeToConnection.end());
		DebugAssert(nodeToConnection.find(m->destinationNode) != nodeToConnection.end());
		delayCalc->OnMsgDelivered(m,nodeToConnection[m->sourceNode],nodeToConnection[m->destinationNode],EM().CurrentTick());
		DebugAssert(nodeToConnection.find(m->sourceNode) != nodeToConnection.end());
		DebugAssert(nodeToConnection.find(m->destinationNode) != nodeToConnection.end());
		SendMsg(nodeToConnection[m->destinationNode], m, 0);
	}
	void RandomLoadNetwork::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		std::vector<std::string> cNames;
		std::vector<NodeID> cNodes;
		for(int i = 0; i < config.SubNodeCount("Connection"); i++)
		{
			const RootConfigNode& cn = Config::GetSubRoot(config,"Connection",i);
			std::string name = Config::GetString(cn,"LinkName");
			NodeID node = (NodeID)Config::GetInt(cn,"NodeName");
			for(size_t j = 0; j < cNames.size(); j++)
			{
				DebugAssert(cNames[j] != name);
			}
			for(size_t j = 0; j < cNodes.size(); j++)
			{
				DebugAssert(cNodes[j] != node);
			}
			cNames.push_back(name);
			cNodes.push_back(node);
		}
		for(int i = 0; i < (int)cNames.size(); i++)
		{
			for(int j = 0; j < ConnectionCount(); j++)
			{
				if(GetConnection(j).Name() == cNames[i])
				{
					nodeToConnection[cNodes[i]] = j;
				}
			}
		}
		DebugAssert(nodeToConnection.size() == cNames.size());
		const RootConfigNode& calcConfig = Config::GetSubRoot(config,"CalculatorConfig");
		if(Config::GetString(calcConfig, "Type") == "Constant")
		{
			delayCalc = new ConstantCalculator;
		}
		else if(Config::GetString(calcConfig, "Type") == "Random")
		{
			delayCalc = new RandomCalculator;
		}
		else if(Config::GetString(calcConfig, "Type") == "RandomLoaded")
		{
			delayCalc = new RandomLoadedCalculator;
		}
		else
		{
			DebugFail("Bad latency calculator type");
		}
		delayCalc->Initialize(calcConfig,(int)cNames.size());
	}
	void RandomLoadNetwork::DumpRunningState(RootConfigNode& node)
	{}
	void RandomLoadNetwork::DumpStats(std::ostream& out)
	{}
	void RandomLoadNetwork::RecvMsg(const BaseMsg* msg, int connectionID)
	{
		DebugAssert(msg);
		DebugAssert(msg->Type() == mt_Network);
		const NetworkMsg* m = (const NetworkMsg*)msg;
		DebugAssert(nodeToConnection.find(m->sourceNode) != nodeToConnection.end());
		DebugAssert(nodeToConnection.find(m->destinationNode) != nodeToConnection.end());
		// if we are overriding the source node, then it's ok for nodeToConnection[m->sourceNode]!=connectionID
		DebugAssert((nodeToConnection[m->sourceNode]==connectionID) || (m->getIsOverrideSource()));
		TimeDelta delay = delayCalc->CalcTime(m,nodeToConnection[m->sourceNode],nodeToConnection[m->destinationNode],EM().CurrentTick());
		delayCalc->OnMsgSend(m,nodeToConnection[m->sourceNode],nodeToConnection[m->destinationNode],EM().CurrentTick());
		CBDeliverMsg::FunctionType* f = cbDeliverMsg.Create();
		f->Initialize(this,m);
		EM().ScheduleEvent(f,delay);
	}
}
