#include "NetworkMemoryInterface.h"
#include "../Debug.h"
#include "../Messages/AllMessageTypes.h"
#include "../Configuration.h"
#include "../Connection.h"
#include "../EventManager.h"
#include "Config.h"

namespace Memory
{
	void NetworkMemoryInterface::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		nodeID = Config::GetInt(config,"NodeID");
		networkConnectionID = memoryConnectionID = -1;
		for(int i = 0; i < ConnectionCount(); i++)
		{
			if(GetConnection(i).Name() == "NetworkConnection")
			{
				networkConnectionID = i;
			}
			else if(GetConnection(i).Name() == "MemoryConnection")
			{
				memoryConnectionID = i;
			}
			else
			{
				DebugFail("Unexpected connection identifier");
			}
		}
		DebugAssert(networkConnectionID != -1 && memoryConnectionID != -1);
	}
	void NetworkMemoryInterface::DumpRunningState(RootConfigNode& node)
	{}
	void NetworkMemoryInterface::DumpStats(std::ostream& out)
	{}
	void NetworkMemoryInterface::RecvMsg(const BaseMsg* msg, int connectionID)
	{
		DebugAssert(msg);
		if(connectionID == networkConnectionID)
		{
			DebugAssert(msg->Type() == mt_Network);
			const NetworkMsg* n = (const NetworkMsg*)msg;
			DebugAssert(n->destinationNode == nodeID);
			const BaseMsg* payload = n->payloadMsg;
			DebugAssert(payload);
			DebugAssertWithMessageID(sourceTable.find(payload->MsgID()) == sourceTable.end(),payload->MsgID());
			sourceTable[payload->MsgID()] = n->sourceNode;
			SendMsg(memoryConnectionID, payload, 0);
			EM().DisposeMsg(n);
		}
		else if(connectionID == memoryConnectionID)
		{
			DebugAssert(msg->IsResponse());
			MessageID referenceID;
			switch(msg->Type())
			{
			case(mt_ReadResponse):
				referenceID = ((const ReadResponseMsg*)msg)->solicitingMessage;
				break;
			case(mt_WriteResponse):
				referenceID = ((const WriteResponseMsg*)msg)->solicitingMessage;
				break;
			default:
				DebugFail("bad from-memory msg type");
			}
			DebugAssert(sourceTable.find(referenceID) != sourceTable.end());
			NetworkMsg* n = EM().CreateNetworkMsg(getDeviceID(), msg->GeneratingPC());
			n->payloadMsg = msg;
			n->sourceNode = nodeID;
			n->destinationNode = sourceTable[referenceID];
			sourceTable.erase(referenceID);
			SendMsg(networkConnectionID, n, 0);
		}
		else
		{
			DebugFail("Invalid connection ID");
		}
	}
}
