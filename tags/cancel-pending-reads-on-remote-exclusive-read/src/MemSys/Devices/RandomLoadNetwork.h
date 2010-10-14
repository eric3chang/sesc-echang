#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"
#include "../MSTypes.h"
#include "Config.h"
#include <vector>

namespace Memory
{
	class NetworkMsg;
	class BaseMsg;
	class RootConfigNode;
	class RandomLoadNetwork : public BaseMemDevice
	{
		class MsgDelayCalculator
		{
		public:
			virtual TimeDelta CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time) = 0;
			virtual void OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time) = 0;
			virtual void OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time) = 0;
			virtual void Initialize(const RootConfigNode& config, int nodeCount) = 0;
			virtual ~MsgDelayCalculator() {}
		};
		class ConstantCalculator : public MsgDelayCalculator
		{
			TimeDelta timeTaken;
			TimeDelta perByte;
		public:
			virtual TimeDelta CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void Initialize(const RootConfigNode& config, int nodeCount);
		};
		class RandomCalculator : public MsgDelayCalculator
		{
			TimeDelta initialTime;
			TimeDelta randomRange;
			TimeDelta perByte;
			bool enforceInOrder;
			std::vector<std::vector<TickTime> > mostRecentMsg;//arrival time, not send time
		public:
			virtual TimeDelta CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void Initialize(const RootConfigNode& config, int nodeCount);
		};
		class RandomLoadedCalculator : public MsgDelayCalculator
		{
			TimeDelta initialTime;
			TimeDelta randomRange;
			float timePerPacket;
			TimeDelta perByte;
			int packetsInTransit;
			bool enforceInOrder;
			std::vector<std::vector<TickTime> > mostRecentMsg;//arrival time, not send time
		public:
			virtual TimeDelta CalcTime(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void OnMsgSend(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void OnMsgDelivered(const NetworkMsg* msg, int fromNode, int toNode, TickTime time);
			virtual void Initialize(const RootConfigNode& config, int nodeCount);
		};
		MsgDelayCalculator* delayCalc;
		HashMap<NodeID, int> nodeToConnection;

		void DeliverMsg(const NetworkMsg* m);
		typedef PooledFunctionGenerator<StoredClassFunction1<RandomLoadNetwork,const NetworkMsg*, &RandomLoadNetwork::DeliverMsg> > CBDeliverMsg;
		CBDeliverMsg cbDeliverMsg;
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
