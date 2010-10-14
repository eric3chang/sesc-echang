#include "SnoopyBus.h"
#include "../Connection.h"
#include "../EventManager.h"
#include "../Configuration.h"
#include "../Messages/AllMessageTypes.h"
#include <cstdlib>

namespace Memory
{
	SnoopyBus::BusArbitration::BusArbitration(int remoteConnection)
	{
		DebugAssert(remoteConnection >= 0);
		this->remoteConnection = remoteConnection;
	}
	int SnoopyBus::BusArbitration::RemoteConnection() const
	{
		return remoteConnection;
	}
	SnoopyBus::RandomArbitration::RandomArbitration(int remoteConnection)
		: BusArbitration(remoteConnection)
	{}
	int SnoopyBus::RandomArbitration::SelectNext(const std::vector<bool>& pending)
	{
		DebugAssert(pending.size() > 0);
		int countPending = 0;
		for(size_t i = 0; i < pending.size(); i++)
		{
			if(pending[i])
			{
				countPending++;
			}
		}
		DebugAssert(countPending > 0);
		int selection = rand() % countPending;
		for(size_t i = 0; i < pending.size(); i++)
		{
			if(pending[i])
			{
				if(selection == 0)
				{
					return (int)i;
				}
				selection--;
			}
		}
		DebugFail("Arbitration Selection Failed");
		return -1;
	}
	SnoopyBus::RemoteFirstRandomArbitration::RemoteFirstRandomArbitration(int remoteConnection)
		: RandomArbitration(remoteConnection)
	{}
	int SnoopyBus::RemoteFirstRandomArbitration::SelectNext(const std::vector<bool> &pending)
	{
		DebugAssert(pending.size() > (size_t)RemoteConnection());
		if(pending[RemoteConnection()])
		{
			return RemoteConnection();
		}
		else
		{
			return RandomArbitration::SelectNext(pending);
		}
	}
	SnoopyBus::ConstantLatencyCalculator::ConstantLatencyCalculator(Memory::TimeDelta dt)
	{
		this->dt = dt;
	}
	TimeDelta SnoopyBus::ConstantLatencyCalculator::CalculateTime(const BaseMsg* m)
	{
		DebugAssert(m);
		return dt;
	}
	SnoopyBus::LinearLatencyCalculator::LinearLatencyCalculator(Memory::TimeDelta fixed, Memory::TimeDelta perByte)
	{
		fixedDt = fixed;
		this->perByte = perByte;
	}
	TimeDelta SnoopyBus::LinearLatencyCalculator::CalculateTime(const Memory::BaseMsg *m)
	{
		DebugAssert(m);
		return fixedDt + m->MsgSize() * perByte;
	}
	SnoopyBus::TransferData::TransferData(){}
	SnoopyBus::TransferData::TransferData(int responses, const Memory::BaseMsg *msg, int connection)
	{
		pendingResponses = responses;
		solicitingMsg = msg;
		solicitingConnection = connection;
		satisfied = false;
	}
	void SnoopyBus::AttemptTransfer()
	{
		bool transferFound = false;
		for(size_t i = 0; i < waitingMsgs.size(); i++)
		{
			pendingMsgs[i] = false;
			for(size_t j = 0; j < waitingMsgs[i].size(); j++)
			{
				if(pendingTransfers.find(GetRelevantAddr(waitingMsgs[i][j])) == pendingTransfers.end())
				{
					transferFound = true;
					pendingMsgs[i] = true;
					msgIndex[i] = (int)j;
					break;
				}
			}
		}
		if(!transferFound)
		{
			transferInProgress = false;
			return;
		}
		transferInProgress = true;
		int arbitrationResult = arbitrator->SelectNext(pendingMsgs);
		DebugAssert(arbitrationResult >= 0 && arbitrationResult < (int)waitingMsgs.size());
		int choice = msgIndex[arbitrationResult];
		DebugAssert(choice >= 0 && choice < (int)waitingMsgs[arbitrationResult].size());
		const BaseMsg* m = waitingMsgs[arbitrationResult][choice];
		DebugAssert(m);
		waitingMsgs[arbitrationResult][choice] = NULL;
		waitingMsgs[arbitrationResult].erase(waitingMsgs[arbitrationResult].begin() + choice);
		BroadcastMsg(m,arbitrationResult);
	}
	void SnoopyBus::ProcessResponse(const BaseMsg* msg, int connectionID)
	{
		DebugAssert(msg);
		Address addr = GetRelevantAddr(msg);
		DebugAssert(msg->IsResponse());
		DebugAssert(pendingTransfers.find(addr) != pendingTransfers.end());
		if(!pendingTransfers[addr].satisfied)
		{
			switch(msg->Type())
			{
			case(mt_ReadResponse):
				if(((const ReadResponseMsg*)msg)->satisfied)
				{
					pendingTransfers[addr].satisfied = true;
				}
				break;
			case(mt_WriteResponse):
				break;
			case(mt_EvictionResponse):
				break;
			case(mt_InvalidateResponse):
				if(((const InvalidateResponseMsg*)msg)->blockAttached)
				{
					pendingTransfers[addr].satisfied = true;
				}
				break;
			case(mt_Read):
			   DebugFail("Should not be here");
			   break;
         case(mt_Eviction):
            DebugFail("Should not be here");
            break;
         case(mt_EvictionBusyAck):
            DebugFail("Should not be here");
            break;
         case(mt_Invalidate):
            DebugFail("Should not be here");
            break;
         case(mt_Network):
            DebugFail("Should not be here");
            break;
         case(mt_Write):
            DebugFail("Should not be here");
            break;
         case(mt_MemAccessComplete):
            DebugFail("Should not be here");
            break;
			} // switch(msg->Type())
			pendingTransfers[addr].pendingResponses--;
			if(pendingTransfers[addr].satisfied)
			{
				TimeDelta delay = latencyCalculator->CalculateTime(msg);
				GetConnection(pendingTransfers[addr].solicitingConnection).SendMsg(msg,delay);
				CBAttemptTransfer::FunctionType* f = cbAttemptTransfer.Create();
				f->Initialize(this);
				EM().ScheduleEvent(f,delay);
				if(pendingTransfers[addr].pendingResponses == 0 || (pendingTransfers[addr].pendingResponses == 1 && pendingTransfers[addr].solicitingConnection != remoteConnection))
				{
					EM().DisposeMsg(pendingTransfers[addr].solicitingMsg);
					pendingTransfers.erase(addr);
				}
			}
			else
			{
				if(pendingTransfers[addr].pendingResponses == 1 && pendingTransfers[addr].solicitingConnection != remoteConnection)
				{
					EM().DisposeMsg(msg);
					GetConnection(remoteConnection).SendMsg(EM().ReplicateMsg(pendingTransfers[addr].solicitingMsg),0);
				}
				else if(pendingTransfers[addr].pendingResponses == 0)
				{
					TimeDelta delay = latencyCalculator->CalculateTime(msg);
					GetConnection(pendingTransfers[addr].solicitingConnection).SendMsg(msg,delay);
					CBAttemptTransfer::FunctionType* f = cbAttemptTransfer.Create();
					f->Initialize(this);
					EM().ScheduleEvent(f,delay);
					EM().DisposeMsg(pendingTransfers[addr].solicitingMsg);
					pendingTransfers.erase(addr);
				}
			}
		}
		else
		{
			EM().DisposeMsg(msg);
			pendingTransfers[addr].pendingResponses--;
			if(pendingTransfers[addr].pendingResponses == 0 || (pendingTransfers[addr].pendingResponses == 1 && pendingTransfers[addr].solicitingConnection != remoteConnection))
			{
				TimeDelta delay = latencyCalculator->CalculateTime(msg);
				CBAttemptTransfer::FunctionType* f = cbAttemptTransfer.Create();
				f->Initialize(this);
				EM().ScheduleEvent(f,delay);
				EM().DisposeMsg(pendingTransfers[addr].solicitingMsg);
				pendingTransfers.erase(addr);
			}
		}
	}
	void SnoopyBus::BroadcastMsg(const BaseMsg* msg, int connectionID)
	{
		DebugAssert(!msg->IsResponse());
		for(int i = 0; i < ConnectionCount(); i++)
		{
			if(i != connectionID && i != remoteConnection)
			{
				GetConnection(i).SendMsg(EM().ReplicateMsg(msg));
			}
		}
		pendingTransfers[GetRelevantAddr(msg)] = TransferData(ConnectionCount() - 1,msg,connectionID);
	}
	Address SnoopyBus::GetRelevantAddr(const BaseMsg* msg)
	{
		Address relevantAddr;
		switch(msg->Type())
		{
		case(mt_Read):
			relevantAddr = ((const ReadMsg*)msg)->addr;
			break;
		case(mt_Write):
			relevantAddr = ((const WriteMsg*)msg)->addr;
			break;
		case(mt_Invalidate):
			relevantAddr = ((const InvalidateMsg*)msg)->addr;
			break;
		case(mt_Eviction):
			relevantAddr = ((const EvictionMsg*)msg)->addr;
			break;
		case(mt_ReadResponse):
			relevantAddr = ((const ReadResponseMsg*)msg)->addr;
			break;
		case(mt_WriteResponse):
			relevantAddr = ((const WriteResponseMsg*)msg)->addr;
			break;
		case(mt_InvalidateResponse):
			relevantAddr = ((const InvalidateResponseMsg*)msg)->addr;
			break;
		case(mt_EvictionResponse):
			relevantAddr = ((const EvictionResponseMsg*)msg)->addr;
			break;
		default:
			DebugFail("Msg type not supported");
		}
		return relevantAddr;
	}
	void SnoopyBus::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		arbitrator = NULL;
		remoteConnection = -1;
		for(size_t i = 0; i < connectionSet.size(); i++)
		{
			waitingMsgs.push_back(std::vector<const BaseMsg*>());
			pendingMsgs.push_back(false);
			msgIndex.push_back(0);
			if(connectionSet[i]->Name() == "RemoteConnection")
			{
				DebugAssert(remoteConnection == -1);
				remoteConnection = (int)i;
			}
			else if(connectionSet[i]->Name() == "LocalConnection")
			{
				localConnectionSet.insert((int)i);
			}
			else
			{
				DebugFail("Invalid connection type specified");
			}
		}
		DebugAssert(remoteConnection != -1);
		DebugAssert(localConnectionSet.size() > 0);
		DebugAssert(config.ContainsSubNode("ArbitrationType") && config.SubNode("ArbitrationType").IsA<StringConfigNode>());
		DebugAssert(config.ContainsSubNode("LatencyCalculation") && config.SubNode("LatencyCalculation").IsA<StringConfigNode>());
		std::string arbType, latCalc, msgSel;
		arbType = ((const StringConfigNode&)config.SubNode("ArbitrationType")).Value();
		latCalc = ((const StringConfigNode&)config.SubNode("LatencyCalculation")).Value();
		if(arbType == "Random")
		{
			arbitrator = new RandomArbitration(remoteConnection);
		}
		else if (arbType == "RemoteFirstRandom")
		{
			arbitrator = new RemoteFirstRandomArbitration(remoteConnection);
		}
		else
		{
			DebugFail("Unknown arbitration type");
		}
		if(latCalc == "Constant")
		{
			DebugAssert(config.ContainsSubNode("TimePerTransfer") && config.SubNode("TimePerTransfer").IsA<IntConfigNode>());
			TimeDelta delay = (TimeDelta)((const IntConfigNode&)config.SubNode("TimePerTransfer")).Value();
			latencyCalculator = new ConstantLatencyCalculator(delay);
		}
		else if(latCalc == "Linear")
		{
			DebugAssert(config.ContainsSubNode("TimePerTransfer") && config.SubNode("TimePerTransfer").IsA<IntConfigNode>());
			DebugAssert(config.ContainsSubNode("TimePerByte") && config.SubNode("TimePerByte").IsA<IntConfigNode>());
			TimeDelta delay = (TimeDelta)((const IntConfigNode&)config.SubNode("TimePerTransfer")).Value();
			TimeDelta perByte = (TimeDelta)((const IntConfigNode&)config.SubNode("TimePerByte")).Value();
			latencyCalculator = new LinearLatencyCalculator(delay,perByte);
		}
		else
		{
			DebugFail("Unknown delay calc");
		}
		DebugAssert(ConnectionCount() == (int)waitingMsgs.size());
		transferInProgress = false;
	}
	void SnoopyBus::DumpRunningState(RootConfigNode& node){}
	void SnoopyBus::DumpStats(std::ostream& out){}
	void SnoopyBus::RecvMsg(const BaseMsg* msg, int connectionID)
	{
		DebugAssert(msg);
		DebugAssert(connectionID >= 0 && connectionID < ConnectionCount());
		if(msg->IsResponse())
		{
			ProcessResponse(msg,connectionID);
		}
		else
		{
			waitingMsgs[connectionID].push_back(msg);
			if(!transferInProgress)
			{
				AttemptTransfer();
			}
		}
	}
}
