#pragma once
#include "BaseMemDevice.h"
#include "../StoredFunctionCall.h"
#include "../HashContainers.h"

namespace Memory
{
	class SnoopyBus : public BaseMemDevice
	{
		class BusArbitration
		{
			int remoteConnection;
		public:
         // added virtual destructor to eliminate constructor warnings
         // Eric Chang 2010/07/30
			virtual ~BusArbitration(){};
			BusArbitration(int remoteConnection);
			virtual int SelectNext(const std::vector<bool>& pending) = 0;

			int RemoteConnection() const;
		};
		class RandomArbitration : public BusArbitration
		{
		public:
			RandomArbitration(int remoteConnection);
			virtual int SelectNext(const std::vector<bool>& pending);
		};
		class RemoteFirstRandomArbitration : public RandomArbitration
		{
		public:
			RemoteFirstRandomArbitration(int remoteConnection);
			virtual int SelectNext(const std::vector<bool>& pending);
		};
		class LatencyCalculator
		{
		public:
		   // added virtual destructor to eliminate constructor warnings
         // Eric Chang 2010/07/30
		   virtual ~LatencyCalculator(){}
			virtual TimeDelta CalculateTime(const BaseMsg* m) = 0;
		};
		class ConstantLatencyCalculator : public LatencyCalculator
		{
			TimeDelta dt;
		public:
			ConstantLatencyCalculator(TimeDelta dt);
			virtual TimeDelta CalculateTime(const BaseMsg* m);
		};
		class LinearLatencyCalculator : public LatencyCalculator
		{
			TimeDelta fixedDt;
			TimeDelta perByte;
		public:
			LinearLatencyCalculator(TimeDelta fixed, TimeDelta perByte);
			virtual TimeDelta CalculateTime(const BaseMsg* m);
		};

		class TransferData
		{
		public:
			TransferData();
			TransferData(int responses, const BaseMsg* msg, int connection);
			int pendingResponses;
			const BaseMsg* solicitingMsg;
			int solicitingConnection;
			bool satisfied;
		};

		BusArbitration* arbitrator;
		LatencyCalculator* latencyCalculator;

		std::vector<std::vector<const BaseMsg*> > waitingMsgs;
		HashMap<Address,TransferData> pendingTransfers;
		
		HashSet<int> localConnectionSet;
		int remoteConnection;

		std::vector<bool> pendingMsgs;
		std::vector<int> msgIndex;

		bool transferInProgress;

		void AttemptTransfer();
		typedef PooledFunctionGenerator<StoredClassFunction0<SnoopyBus,&SnoopyBus::AttemptTransfer> > CBAttemptTransfer;
		CBAttemptTransfer cbAttemptTransfer;

		void ProcessResponse(const BaseMsg* msg, int connectionID);
		void BroadcastMsg(const BaseMsg* msg, int connectionID);
		Address GetRelevantAddr(const BaseMsg* msg);
	public:
		virtual void Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet);
		virtual void DumpRunningState(RootConfigNode& node);
		virtual void DumpStats(std::ostream& out);
		virtual void RecvMsg(const BaseMsg* msg, int connectionID);
	};
}
