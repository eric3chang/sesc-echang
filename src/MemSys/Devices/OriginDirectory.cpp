// toggles debug messages
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_COUNTERS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_MSG_COUNT
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_DIRECTORY_EXCLUSIVE_READS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_EVICTION
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_LOCAL_READS
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
//#define MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_REMOTE_READS

// used in AutoDetermineDestSendMsg's member function calling
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

#include "OriginDirectory.h"
#include "../Debug.h"
#include "../MSTypes.h"
#include "../Messages/AllMessageTypes.h"
#include "../Configuration.h"
#include "../EventManager.h"
#include "../Connection.h"

// debugging utilities
#include <sstream>
#include "to_string.h"

using std::cerr;
using std::cout;
using std::endl;
using std::vector;

namespace Memory
{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_COUNTERS
   int threeStageDirectoryEraseDirectoryShareCounter = 0;
#endif

	const int OriginDirectory::InvalidInvalidAcksReceived = 0;

	NodeID OriginDirectory::HashedPageCalculator::CalcNodeID(Address addr) const
	{
		int index = ((int)(addr / pageSize) % elementCount);
		DebugAssert(index >= 0 && index < (int)nodeSet.size());
		return nodeSet[index];
	}

	void OriginDirectory::HashedPageCalculator::Initialize(const RootConfigNode& node)
	{
	   // "PageSize" should be a multiple of block size
	   // usually, PageSize would be the same as block size
		pageSize = Config::GetInt(node,"PageSize");
		elementCount = Config::GetSetSize(node,"NodeIDSet");
		for(int i = 0; i < elementCount; i++)
		{
			nodeSet.push_back(Config::GetInt(node,"NodeIDSet",0,i));
		}
	}

	OriginDirectory::CacheData::CacheData() :
						cacheState(cs_Invalid),
						readRequestState(rrs_NoPendingReads),
						isCanceledReadRequest(false),
						firstReply(NULL),
						firstReplySrc(InvalidNodeID),
						invalidAcksReceived(InvalidInvalidAcksReceived),
						pendingExclusiveRead(NULL),
						pendingSharedRead(NULL)
	{}

	OriginDirectory::CacheState OriginDirectory::CacheData::GetCacheState()
	{
		return cacheState;
	}

	void OriginDirectory::CacheData::SetCacheState(CacheState newCacheState)
	{
		cacheState = newCacheState;
	}

	void OriginDirectory::dump(HashMap<Memory::MessageID, const Memory::BaseMsg*> &m)
	{
		DumpMsgTemplate<Memory::MessageID>(m);
	}
	void OriginDirectory::dump(HashMap<Address,LookupData<BaseMsg> > &m)
	{
		DumpLookupDataTemplate<Address>(m);
	}

	void OriginDirectory::ClearTempCacheData(CacheData& cacheData)
   {
   	//EM().DisposeMsg(cacheData.firstReply);
   	cacheData.firstReply = NULL;
   	cacheData.firstReplySrc = InvalidNodeID;
   	cacheData.invalidAcksReceived = InvalidInvalidAcksReceived;
		// cannot dispose secondReply because we usually don't have secondReply
		cacheData.secondReply = NULL;
   }

   void OriginDirectory::ClearTempDirectoryData(DirectoryData& directoryData)
   {
   	//EM().DisposeMsg(directoryData.firstRequest);
   	directoryData.firstRequest = NULL;
   	directoryData.firstRequestSrc = InvalidNodeID;
   	directoryData.previousOwner = InvalidNodeID;
		// can't dispose secondRequest here since we don't always have a secondRequest
   	////EM().DisposeMsg(directoryData.secondRequest);
   	directoryData.secondRequest = NULL;
   	directoryData.secondRequestSrc = InvalidNodeID;
   }

   OriginDirectory::CacheData& OriginDirectory::GetCacheData(Address addr)
	{
		return cacheDataMap[addr];
	}

   OriginDirectory::CacheData& OriginDirectory::GetCacheData(const BaseMsg* m)
	{
		Address currentAddress = BaseMemDevice::GetAddress(m);
		return cacheDataMap[currentAddress];
	}

   OriginDirectory::DirectoryData& OriginDirectory::GetDirectoryData(Address addr)
	{
		return directoryDataMap[addr];
	}

   OriginDirectory::DirectoryData& OriginDirectory::GetDirectoryData(const BaseMsg* m)
	{
		Address currentAddress = BaseMemDevice::GetAddress(m);
		return directoryDataMap[currentAddress];
	}

	void OriginDirectory::PerformMemoryReadResponseCheck(const ReadResponseMsg *m, NodeID src)
	{
		DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());
		DebugAssertWithMessageID(pendingMemoryReadAccesses.find(m->solicitingMessage)!=pendingMemoryReadAccesses.end(), m->MsgID());
		const ReadMsg* rm = pendingMemoryReadAccesses[m->solicitingMessage];
		//EM().DisposeMsg(rm);
		pendingMemoryReadAccesses.erase(m->solicitingMessage);
	}

	void OriginDirectory::PerformMemoryWriteResponseCheck(const WriteResponseMsg *m, NodeID src)
	{
		DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());
		DebugAssertWithMessageID(pendingMemoryWriteAccesses.find(m->solicitingMessage)!=pendingMemoryWriteAccesses.end(), m->MsgID());
		const WriteMsg* wm = pendingMemoryWriteAccesses[m->solicitingMessage];
		//EM().DisposeMsg(wm);
		pendingMemoryWriteAccesses.erase(m->solicitingMessage);
	}

	void OriginDirectory::RecvMsgCache(const BaseMsg *msg, NodeID src)
	{
		Address addr = BaseMemDevice::GetAddress(msg);
		
		if (msg->Type()==mt_CacheNak)
		{
			const CacheNakMsg* m = (const CacheNakMsg*)msg;
			OnCacheCacheNak(m, src);
		}
		else if (msg->Type()==mt_Eviction)
		{
			const EvictionMsg* m = (const EvictionMsg*)msg;
			DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());
			OnCacheEviction(m, src);
		}
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			OnCacheIntervention(m, src);
		}
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			OnCacheInvalidate(m, src);
		}
		else if (msg->Type()==mt_InvalidateAck)
		{
			const InvalidateAckMsg* m = (const InvalidateAckMsg*)msg;
			OnCacheInvalidateAck(m, src);
		}
		else if (msg->Type()==mt_InvalidateResponse)
		{
			const InvalidateResponseMsg* m = (const InvalidateResponseMsg*)msg;
			OnCacheInvalidateResponse(m, src);
		}
		else if (msg->Type()==mt_Read)
		{
			DebugAssertWithMessageID(src==InvalidNodeID, msg->MsgID());
			const ReadMsg* m = (const ReadMsg*) msg;
			OnCacheRead(m);
		}
		/*
		else if (src!=InvalidNodeID && (msg->Type()==mt_ReadResponse||msg->Type()==mt_ReadReply||msg->Type()==mt_SpeculativeReply))
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*) msg;
			//DebugAssertWithMessageID(src!=InvalidNodeID, m->solicitingMessage);
			OnCacheReadResponse(m, src);
		}
		*/
		else if (msg->Type()==mt_SpeculativeReply)
		{
			const SpeculativeReplyMsg* m = (const SpeculativeReplyMsg*)msg;
			OnCacheSpeculativeReply(m, src);
		}
		else if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			OnCacheReadResponse(m, src);
		}
		else if (msg->Type()==mt_ReadReply)
		{
			const ReadReplyMsg* m = (const ReadReplyMsg*)msg;
			OnCacheReadReply(m, src);
		}
		else if (msg->Type()==mt_WritebackAck)
		{
			const WritebackAckMsg* m = (const WritebackAckMsg*)msg;
			OnCacheWritebackAck(m, src);
		}
		else
		{
			CacheData& cacheData = GetCacheData(msg);
			OnCache(msg, src, cacheData);
		}
	}

	void OriginDirectory::RecvMsgDirectory(const BaseMsg *msg, NodeID src, bool isFromMemory)
	{
		if (msg->Type()==mt_DirectoryNak)
		{
			DebugAssertWithMessageID(!isFromMemory, msg->MsgID());
			const DirectoryNakMsg* m = (const DirectoryNakMsg*)msg;
			OnDirectoryDirectoryNak(m, src);
		}
		else if (msg->Type()==mt_Read)
		{
			DebugAssertWithMessageID(!isFromMemory, msg->MsgID());
			const ReadMsg* m = (const ReadMsg*)msg;
			OnDirectoryRead(m, src);
		}
		else if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DirectoryData& directoryData = GetDirectoryData(m->addr);
			DebugAssertWithMessageID(isFromMemory, m->solicitingMessage);
			OnDirectory(m, src, directoryData, isFromMemory);
		}
		else if (msg->Type()==mt_Transfer)
		{
			const TransferMsg* m = (const TransferMsg*)msg;
			DirectoryData& directoryData = GetDirectoryData(m->addr);
			OnDirectory(m, src, directoryData, isFromMemory);
		}
		else if (msg->Type()==mt_Writeback)
		{
			DebugAssertWithMessageID(!isFromMemory, msg->MsgID());
			const WritebackMsg* m = (const WritebackMsg*)msg;
			OnDirectoryWriteback(m, src);
		}
		else if (msg->Type()==mt_WritebackRequest)
		{
			DebugAssertWithMessageID(!isFromMemory, msg->MsgID());
			const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;
			OnDirectoryWritebackRequest(m, src);
		}
		else if (msg->Type()==mt_WriteResponse)
		{
			const WriteResponseMsg* m = (const WriteResponseMsg*)msg;
			DebugAssertWithMessageID(isFromMemory, m->solicitingMessage);
			PerformMemoryWriteResponseCheck(m, src);
		}
		else
		{
			DirectoryData& directoryData = GetDirectoryData(msg);
			OnDirectory(msg, src, directoryData, isFromMemory);
		}
	}

	void OriginDirectory::ResendRequestToDirectory(ReadMsg *m)
	{
		NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);

		m->isDirectory = true;

		if (dirNode==nodeID)
		{
			// schedule RecvMsgDirectory to be called
			CBRecvMsgDirectory::FunctionType* f = cbRecvMsgDirectory.Create();
			f->Initialize(this, m, nodeID, false);
			EM().ScheduleEvent(f, lookupRetryTime);
		}
		else
		{
			SendMessageToNetwork(m, dirNode);
		}
	}

	void OriginDirectory::SendCacheNak(const InterventionMsg* m)
   {
   	CacheNakMsg* cnk = EM().CreateCacheNakMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeBaseNakMsg(cnk, m);
		cnk->interventionMessage = m->MsgID();
		SendMessageToRemoteCache(cnk, m->newOwner);
   } // SendCacheNak

   void OriginDirectory::SendCacheNak(const ReadMsg* m, NodeID dest)
   {
   	CacheNakMsg* cnk = EM().CreateCacheNakMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeBaseNakMsg(cnk, m);
   	SendMessageToRemoteCache(cnk, dest);
   } // SendCacheNak

   void OriginDirectory::SendDirectoryNak(const InterventionMsg* m)
   {
   	DirectoryNakMsg* dnk = EM().CreateDirectoryNakMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeBaseNakMsg(dnk, m);
   	SendMessageToDirectory(dnk);
   } // SendDirectoryNak

	void OriginDirectory::SendInterventionNaks(const InterventionMsg* im)
	{
		DebugAssertWithMessageID(im->newOwner!=InvalidNodeID, im->solicitingMessage);

		// send nak to directory
		SendDirectoryNak(im);

		// send nak to requester
		SendCacheNak(im);
	}

   void OriginDirectory::SendInvalidateMsg(const ReadMsg* m, NodeID dest, NodeID newOwner)
   {
   	InvalidateMsg* im = EM().CreateInvalidateMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeInvalidateMsg(im, m);
   	im->newOwner = newOwner;
   	SendMessageToRemoteCache(im, dest);
   } // SendInvalidateMsg

	void OriginDirectory::SendMessageToRemoteCache(const BaseMsg *msgIn, NodeID dest)
	{
		DebugAssertWithMessageID(dest!=InvalidNodeID, msgIn->MsgID());

		BaseMsg* msg = EM().ReplicateMsg(msgIn);

		msg->isCache = true;

		if (dest==nodeID)
		{
			// schedule RecvMsgCache to be called
			CBRecvMsgCache::FunctionType* f = cbRecvMsgCache.Create();
			f->Initialize(this, msg, nodeID);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			SendMessageToNetwork(msg, dest);
		}
	}

	void OriginDirectory::SendMessageToLocalCache(const ReadReplyMsg* msg)
	{
		ReadResponseMsg* m = EM().CreateReadResponseMsg(GetDeviceID(), msg->GeneratingPC());
		EM().InitializeReadResponseMsg(m, msg);
		// set send time to 0 because we are saying that this
			// code in the OriginDirectory is already part of the cache
		SendMsg(localCacheConnectionID, m, localSendTime);
		EM().DisposeMsg(msg);
	}

	void OriginDirectory::SendMessageToDirectory(BaseMsg *msg)
	{
		SendMessageToDirectory(msg, false);
	}

   void OriginDirectory::SendMessageToDirectory(BaseMsg *msg, bool isFromMemory)
   {
   	Address addr = BaseMemDevice::GetAddress(msg);
		NodeID dirNode = directoryNodeCalc->CalcNodeID(addr);
		DebugAssertWithMessageID(dirNode!=InvalidNodeID, msg->MsgID());

		msg->isDirectory = true;

		if(dirNode == nodeID)
		{
			// schedule OnDirectory to be called
			CBRecvMsgDirectory::FunctionType* f = cbRecvMsgDirectory.Create();
			f->Initialize(this, msg, nodeID, isFromMemory);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			SendMessageToNetwork(msg, dirNode);
		}
   }

   /**
   send a memory access complete message
   */
   void OriginDirectory::SendRequestToMemory(const BaseMsg *msg)
   {
   	if (msg->Type()==mt_Read)
   	{
   		DebugAssertWithMessageID(pendingMemoryReadAccesses.find(msg->MsgID())==pendingMemoryReadAccesses.end(),msg->MsgID());
   		const ReadMsg* m = (const ReadMsg*) msg;
   		pendingMemoryReadAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,localSendTime);
   	}
   	else if (msg->Type()==mt_Write)
   	{
   		DebugAssertWithMessageID(pendingMemoryWriteAccesses.find(msg->MsgID())==pendingMemoryWriteAccesses.end(),msg->MsgID());
   		const WriteMsg* m = (const WriteMsg*) msg;
   		pendingMemoryWriteAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,localSendTime);
   	}
   	else
   	{
   		PrintError("SendMemoryRequest", msg, "can't send current message type to memory");
   	}
   }

   void OriginDirectory::SendMessageToNetwork(const BaseMsg *msg, NodeID dest)
   {
   	// only send a network message if we actually need to, otherwise, we should
   		// just schedule the event
   	DebugAssertWithMessageID(dest!=nodeID, msg->MsgID());

   	NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), msg->GeneratingPC());
   	DebugAssertWithMessageID(nm,msg->MsgID());
   	nm->sourceNode = nodeID;
   	nm->destinationNode = dest;
   	nm->payloadMsg = msg;
   	SendMsg(remoteConnectionID, nm, remoteSendTime);
   }

   /**
    * Automatically determines whether to send to local or remote node
    */
   void OriginDirectory::AutoDetermineDestSendMsg(const BaseMsg* msg, NodeID dest, TimeDelta sendTime,
      OriginDirectoryMemFn func, const char* fromMethod, const char* toMethod)
   {
      if (dest==nodeID)
      {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
         PrintDebugInfo(toMethod,*msg,fromMethod,nodeID);
#endif
         CALL_MEMBER_FN(*this,func) (msg,dest);
         return;
      }
      else
      {
         NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),msg->GeneratingPC());
         nm->payloadMsg = msg;
         nm->sourceNode = nodeID;
         nm->destinationNode = dest;
         SendMsg(remoteConnectionID, nm, sendTime);
      }
   }

	void OriginDirectory::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		localCacheConnectionID = localMemoryConnectionID = remoteConnectionID = -1;
		for(int i = 0; i < ConnectionCount(); i++)
		{
			if(GetConnection(i).Name() == "LocalCacheConnection")
			{
				localCacheConnectionID = i;
			}
			else if(GetConnection(i).Name() == "LocalMemoryConnection")
			{
				localMemoryConnectionID = i;
			}
			else if(GetConnection(i).Name() == "RemoteConnection")
			{
				remoteConnectionID = i;
			}
			else
			{
				DebugFail("unexpected connection name");
			}
		}
		DebugAssert(localCacheConnectionID != -1);
		DebugAssert(localMemoryConnectionID != -1);
		DebugAssert(remoteConnectionID != -1);
		localSendTime = Config::GetIntOrElse(1,config,"LocalSendTime");
		remoteSendTime = Config::GetIntOrElse(1,config,"RemoteSendTime");
		lookupRetryTime = Config::GetIntOrElse(1,config,"LookupRetryTime");
		// the time for the requesting node to ask for the data,
		// until the home node has received the data
		lookupTime = Config::GetIntOrElse(1,config,"LookupTime");
		// the time after home node has received the requested data,
		// until the requesting node received the requested data
		satisfyTime = Config::GetIntOrElse(1,config,"SatisfyTime");
		nodeID = (NodeID)Config::GetIntOrElse(1,config,"NodeID");
		const RootConfigNode& dirCalc = Config::GetSubRoot(config,"DirectoryNodeCalculator");
		if(Config::GetString(dirCalc, "Type") == "HashedPageCalculator")
		{
			directoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown directory node calculator type");
		}
		directoryNodeCalc->Initialize(dirCalc);

		messagesReceived = 0;
		cacheNaksReceived = 0;
		cacheEvictionsReceived = 0;
		cacheInterventionsReceived = 0;
		cacheInvalidatesReceived = 0;
		cacheInvalidateAcksReceived = 0;
		cacheInvalidateResponsesReceived = 0;
		cacheSpeculativeRepliesReceived = 0;
		cacheReadsReceived = 0;
		cacheReadResponsesReceived = 0;
		cacheReadRepliesReceived = 0;
		directoryNaksReceived = 0;
		directoryReadsReceived = 0;
		directoryReadResponsesReceived = 0;
		directoryTransfersReceived = 0;
		directoryWritebacksReceived = 0;
		directoryWritebackRequestsReceived = 0;
		directoryWriteResponsesReceived = 0;
	}

	/**
	 * this is used for checkpoint purposes
	 */
	void OriginDirectory::DumpRunningState(RootConfigNode& node)
	{}

	/**
	 * put anything here that you might want to output to the report file
	 */
	void OriginDirectory::DumpStats(std::ostream& out)
	{
		out << DeviceName() << ":localSendTime:" << localSendTime << std::endl;
		out << DeviceName() << ":remoteSendTime:" << remoteSendTime << std::endl;
		out << DeviceName() << ":lookupRetryTime:" << lookupRetryTime << std::endl;
		out << DeviceName() << ":lookupTime:" << lookupTime << std::endl;
		out << DeviceName() << ":satisfyTime:" << satisfyTime << std::endl;
		out << DeviceName() << ":nodeID:" << nodeID << std::endl;
	   out << DeviceName() << ":TotalMessagesReceived:" << messagesReceived << std::endl;
		out << DeviceName() << ":cacheNaksReceived:" << cacheNaksReceived << std::endl;
		out << DeviceName() << ":cacheEvictionsReceived:" << cacheEvictionsReceived << std::endl;
		out << DeviceName() << ":cacheInterventionsReceived:" << cacheInterventionsReceived << std::endl;
		out << DeviceName() << ":cacheInvalidatesReceived:" << cacheInvalidatesReceived << std::endl;
		out << DeviceName() << ":cacheInvalidateAcksReceived:" << cacheInvalidateAcksReceived << std::endl;
		out << DeviceName() << ":cacheInvalidateResponsesReceived:" << cacheInvalidateResponsesReceived << std::endl;
		out << DeviceName() << ":cacheSpeculativeRepliesReceived:" << cacheSpeculativeRepliesReceived << std::endl;
		out << DeviceName() << ":cacheReadResponsesReceived:" << cacheReadResponsesReceived << std::endl;
		out << DeviceName() << ":cacheReadRepliesReceived:" << cacheReadRepliesReceived << std::endl;
		out << DeviceName() << ":directoryNaksReceived:" << directoryNaksReceived << std::endl;
		out << DeviceName() << ":directoryReadsReceived:" << directoryReadsReceived << std::endl;
		out << DeviceName() << ":directoryReadResponsesReceived:" << directoryReadResponsesReceived << std::endl;
		out << DeviceName() << ":directoryTransfersReceived:" << directoryTransfersReceived << std::endl;
		out << DeviceName() << ":directoryWritebacksReceived:" << directoryWritebacksReceived << std::endl;
		out << DeviceName() << ":directoryWritebackRequestsReceived:" << directoryWritebackRequestsReceived << std::endl;
		out << DeviceName() << ":directoryWriteResponsesReceived:" << directoryWriteResponsesReceived << std::endl;
	}

	void OriginDirectory::OnCacheCacheNak(const CacheNakMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheCacheNak",*m,"",src);
#endif
		cacheNaksReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
		/*
		const ReadMsg*& pendingExclusiveRead = cacheData.pendingExclusiveRead;
		const ReadMsg*& pendingSharedRead = cacheData.pendingSharedRead;
		ReadRequestState &state = cacheData.readRequestState;

		switch(state)
		{
		case rrs_PendingSharedRead:
			DebugAssertWithMessageID(pendingSharedRead!=NULL, m->solicitingMessage);
			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage, m->solicitingMessage);
			break;
		case rrs_PendingExclusiveRead:
			DebugAssertWithMessageID(pendingExclusiveRead!=NULL, m->solicitingMessage);
			DebugAssertWithMessageID(pendingExclusiveRead->MsgID()==m->solicitingMessage, m->solicitingMessage);
			break;
		case rrs_PendingSharedReadExclusiveRead:
			DebugAssertWithMessageID(pendingSharedRead!=NULL, m->solicitingMessage);
			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage, m->solicitingMessage);
			break;
		default:
			PrintError("OnCacheCacheNak", m, "Unhandled state");
			break;
		}
		*/
	}

	void OriginDirectory::OnCacheInvalidate(const InvalidateMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheInvalidate",*m,"",src);
#endif
		cacheInvalidatesReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
	}

	void OriginDirectory::OnCacheInvalidateAck(const InvalidateAckMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheInvalidateAck",*m,"",src);
#endif
		cacheInvalidateAcksReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
		/*
		CacheState cacheState = cacheData.GetCacheState();
		const ReadMsg*& pendingExclusiveRead = cacheData.pendingExclusiveRead;
		ReadRequestState& readRequestState = cacheData.readRequestState;
		
		// can't check solicitingMessage of InvalidateAckMsg because it refers to InvalidateMsg,
			// and not the ReadMsg that generated the InvalidateMsg

		switch(readRequestState)
		{
		case rrs_PendingExclusiveRead:
			OnCache(m, src, cacheData);
			if (cacheState==cs_Exclusive)
			{
				readRequestState = rrs_NoPendingReads;
				pendingExclusiveRead = NULL;
			}
			break;
		default:
			PrintError("OnCacheInvAck", m);
		}
		*/
	}

	void OriginDirectory::OnCacheInvalidateResponse(const InvalidateResponseMsg* m, NodeID src)
   {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheInvalidateResponse",*m,"",src);
#endif
		cacheInvalidateResponsesReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
	}

	void OriginDirectory::OnCacheRead(const ReadMsg* m)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheRead",*m,"");
#endif
		cacheReadsReceived++;
		DebugAssertWithMessageID(pendingLocalReads.find(m->MsgID())==pendingLocalReads.end(), m->MsgID());
		pendingLocalReads[m->MsgID()] = m;
		CacheData& cacheData = GetCacheData(m->addr);
		vector<const ReadMsg*>& cacheDataPendingLocalReads = cacheData.cacheDataPendingLocalReads;
		CacheState cacheState = cacheData.GetCacheState();

		cacheDataPendingLocalReads.push_back(m);

		if (cacheState==cs_Invalid || cacheState==cs_Exclusive || cacheState==cs_Shared)
		{
			const ReadMsg* forward = (const ReadMsg*)EM().ReplicateMsg(m);
			OnCache(forward, InvalidNodeID, cacheData);
		}
	}

	/*
	void OriginDirectory::OnCacheRead(const ReadMsg* m)
   {
		CacheData& cacheData = GetCacheData(m->addr);
   	ReadRequestState& state = cacheData.readRequestState;
   	const ReadMsg*& pendingExclusiveRead = cacheData.pendingExclusiveRead;
   	const ReadMsg*& pendingSharedRead = cacheData.pendingSharedRead;

   	switch(state)
   	{
   	case rrs_NoPendingReads:
   		if (!m->requestingExclusive)
   		{
   			state = rrs_PendingSharedRead;
   			DebugAssertWithMessageID(pendingSharedRead==NULL, m->MsgID());
   			pendingSharedRead = m;
   			OnCache(m, InvalidNodeID, cacheData);
   		}
   		else
   		{ // m is requesting exclusive
   			state = rrs_PendingExclusiveRead;
   			DebugAssertWithMessageID(pendingExclusiveRead==NULL, m->MsgID());
   			pendingExclusiveRead = m;
   			OnCache(m, InvalidNodeID, cacheData);
   		}
   		break;
   	case rrs_PendingSharedRead:
   		if (m->requestingExclusive)
   		{
   			state = rrs_PendingSharedReadExclusiveRead;
   		}
   		else
   		{
   			// ignore message if it's a shared read
   		}
   		break;
   	case rrs_PendingSharedReadExclusiveRead:
   		// ignore message
   		break;
   	case rrs_PendingExclusiveRead:
   		// ignore message
   		break;
   	default:
   		PrintError("OnCacheRead",m,state);
   		break;
   	}
   }
   */

	void OriginDirectory::OnCacheReadReply(const ReadReplyMsg* m, NodeID src)
   {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheReadReply",*m,"",src);
#endif
		cacheReadRepliesReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
	}

	void OriginDirectory::OnCacheReadResponse(const ReadResponseMsg* m, NodeID src)
   {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheReadResponse",*m,"",src);
#endif
		cacheReadResponsesReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
		/*
		CacheState cacheState = cacheData.GetCacheState();
   	ReadRequestState& readRequestState = cacheData.readRequestState;
   	const ReadMsg*& pendingExclusiveRead = cacheData.pendingExclusiveRead;
   	const ReadMsg*& pendingSharedRead = cacheData.pendingSharedRead;

		DebugAssertWithMessageID(m->satisfied, m->solicitingMessage);

   	switch(readRequestState)
   	{
   	case rrs_NoPendingReads:
   		// should not receive a read response to cache
   		PrintError("OnDirReadRes", m, readRequestState);
   		break;
   	case rrs_PendingExclusiveRead:
			// must always be satisfied because we use CacheNakMsg for unsatisfied read requests
			DebugAssertWithMessageID(m->satisfied, m->solicitingMessage);
			DebugAssertWithMessageID(pendingSharedRead==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(pendingExclusiveRead!=NULL, m->solicitingMessage);
			DebugAssertWithMessageID(m->solicitingMessage==pendingExclusiveRead->MsgID(), m->solicitingMessage);

   		OnCache(m, src, cacheData);

			if (m->exclusiveOwnership && cacheState==cs_Exclusive)
			{
				readRequestState = rrs_NoPendingReads;
				// message gets disposed in OnCache()
				pendingExclusiveRead = NULL;
			}
   		break;
   	case rrs_PendingSharedRead:
   		// could be exclusive owned because it could be CEX
   		// DebugAssert(!m.isExclusiveOwned)
			// must always be satisfied because we use CacheNakMsg for unsatsified read requests
			DebugAssertWithMessageID(m->satisfied, m->solicitingMessage);

			// solicitingMessage might not have to be messageID? Because it could be a speculativeReply or something else
			
			DebugAssertWithMessageID(m->solicitingMessage==pendingSharedRead->MsgID(), m->solicitingMessage);
			DebugAssertWithMessageID(pendingExclusiveRead==NULL, m->solicitingMessage);

   		OnCache(m, src, cacheData);

			if (cacheState==cs_Shared || cacheState==cs_Exclusive)
			{
   			readRequestState = rrs_NoPendingReads;
				// message gets disposed in OnCache()
   			pendingSharedRead = NULL;
			}
   		break;
   	case rrs_PendingSharedReadExclusiveRead:
			// must always be satisfied because we use CacheNakMsg for unsatsified read requests
			DebugAssertWithMessageID(m->satisfied, m->solicitingMessage);
			DebugAssertWithMessageID(pendingSharedRead!=NULL, m->solicitingMessage);
			DebugAssertWithMessageID(pendingExclusiveRead!=NULL, m->solicitingMessage);
			// can only receive response to the first shared read because we only sent the first shared read
			DebugAssertWithMessageID(m->solicitingMessage==pendingSharedRead->MsgID(), m->solicitingMessage);

			OnCache(m, src, cacheData);

   		if (m->exclusiveOwnership && cacheState==cs_Exclusive)
   		{
   			readRequestState = rrs_NoPendingReads;

				// message gets disposed in OnCache()
   			pendingSharedRead = NULL;
				
   			pendingExclusiveRead = NULL;
   		}
   		else // !m->exclusiveOwnership
   		{
				readRequestState = rrs_PendingExclusiveRead;

				// message gets disposed in OnCache()
				pendingSharedRead = NULL;

				// send the exclusive read request that was queued up
				OnCache(pendingExclusiveRead, InvalidNodeID, cacheData);
				//const BaseMsg* forward = EM().ReplicateMsg(pendingExclusiveRead);
				//SendMessageToDirectory(forward,false);
   		}
   		break;
   	default:
   		PrintError("OnDirReadRes", m, readRequestState);
   		break;
   	}
		*/
   }

	void OriginDirectory::OnCache(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		//CacheState state = cacheData.GetCacheState();
		cacheData.previousMessage = cacheData.currentMessage;
		cacheData.currentMessage = msg->MsgID();
		Address addr = BaseMemDevice::GetAddress(msg);

		switch(cacheData.GetCacheState())
		{
		case cs_Exclusive:
			OnCacheExclusive(msg,src,cacheData);
			break;
		case cs_ExclusiveInterventionEviction:
			OnCacheExclusiveInterventionEviction(msg, src, cacheData);
			break;
		case cs_ExclusiveWaitingForSpeculativeReply:
			OnCacheExclusiveWaitingForSpeculativeReply(msg, src, cacheData);
			break;
		case cs_Invalid:
			OnCacheInvalid(msg,src,cacheData);
			break;
		case cs_Shared:
			OnCacheShared(msg,src,cacheData);
			break;
		case cs_SharedWaitingForSpeculativeReply:
			OnCacheSharedWaitingForSpeculativeReply(msg, src, cacheData);
			break;
		case cs_WaitingForExclusiveReadResponse:
			OnCacheWaitingForExclusiveReadResponse(msg, src, cacheData);
			break;
		case cs_WaitingForExclusiveResponseAck:
			OnCacheWaitingForExclusiveResponseAck(msg, src, cacheData);
			break;
		case cs_WaitingForIntervention:
			OnCacheWaitingForIntervention(msg, src, cacheData);
			break;
		case cs_WaitingForKInvalidatesJInvalidatesReceived:
			OnCacheWaitingForKInvalidatesJInvalidatesReceived(msg, src, cacheData);
			break;
		case cs_WaitingForSharedReadResponse:
			OnCacheWaitingForSharedReadResponse(msg, src, cacheData);
			break;
		case cs_WaitingForSharedResponseAck:
			OnCacheWaitingForSharedResponseAck(msg, src, cacheData);
			break;
		case cs_WaitingForWritebackBusyAck:
			OnCacheWaitingForWritebackBusyAck(msg, src, cacheData);
			break;
		case cs_WaitingForWritebackResponse:
			OnCacheWaitingForWritebackResponse(msg, src, cacheData);
			break;
		default:
			PrintError("OnCache",msg);
			break;
		}
	}	// OnCache

   void OriginDirectory::OnCacheCleanExclusive(const BaseMsg* msg, NodeID src, CacheData& cacheData)
   {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheCleanEx",*msg,"",src);
#endif
		//const BaseMsg*& firstReplyBase = cacheData.firstReply;
   	//DebugAssertWithMessageID(firstReplyBase!=NULL, msg->MsgID());
   	//NodeID& firstReplySrc = cacheData.firstReplySrc;
		//DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, firstReplyBase->MsgID());
		//const EvictionMsg*& secondReply = cacheData.secondReply;

		if (msg->Type()==mt_Eviction)
		{
			const EvictionMsg* m = (const EvictionMsg*)msg;			
   		const BaseMsg*& firstReplyBase = cacheData.firstReply;
   		DebugAssertWithMessageID(pendingEviction.find(m->addr)==pendingEviction.end(), msg->MsgID());
   		DebugAssertWithMessageID(firstReplyBase==NULL, msg->MsgID());
   		DebugAssertWithMessageID(src==InvalidNodeID, msg->MsgID());

			// send eviction response to cache
			EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(erm, m);
			SendMsg(localCacheConnectionID, erm, localSendTime);

			cacheData.SetCacheState(cs_Invalid);
		}
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(src==dirNode, m->solicitingMessage);
   		//CacheState state = cacheData.GetCacheState();

   		//const InterventionMsg* m = (const InterventionMsg*)msg;
			DebugAssertWithMessageID(pendingEviction.find(m->addr)==pendingEviction.end(), m->solicitingMessage);

   		if (!m->requestingExclusive)
   		{
   			// send shared read ack to the requester, which is the new owner
   			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeReadResponseMsg(rrm, m);
   			rrm->blockAttached = false;
   			rrm->exclusiveOwnership = false;
				rrm->interventionMessage = m->MsgID();
   			rrm->satisfied = true;
				SendMessageToRemoteCache(rrm, m->newOwner);

   			// send shared transfer to directory
   			TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeEvictionMsg(tm, m);
   			tm->blockAttached = false;
   			tm->isShared = true;
   			SendMessageToDirectory(tm, false);

   			ClearTempCacheData(cacheData);
			
   			cacheData.SetCacheState(cs_Shared);
   		} // if (!m->requestingExclusive)
   		else
   		{
   			// send exclusive read ack to the requester, which is the new owner
   			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeReadResponseMsg(rrm, m);
   			rrm->blockAttached = false;
   			rrm->exclusiveOwnership = true;
				rrm->interventionMessage = m->MsgID();
   			rrm->satisfied = true;
				SendMessageToRemoteCache(rrm, m->newOwner);

   			// send dirty transfer to directory
   			TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeEvictionMsg(tm, m);
   			tm->blockAttached = false;
   			tm->isDirty = true;
   			SendMessageToDirectory(tm, false);

   			ClearTempCacheData(cacheData);
				
   			cacheData.SetCacheState(cs_Invalid);
   		} // else m is requesting exclusive
		} // if (msg->Type()==mt_Intervention)
		else
		{
			PrintError("OnCacheCleanEx", msg, "Unhandled message type");
		}
   } // OnCacheCleanExclusive

   void OriginDirectory::OnCacheDirtyExclusive(const BaseMsg* msg, NodeID src, CacheData& cacheData)
   {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheDirtyEx",*msg,"",src);
#endif
   	//NodeID& firstReplySrc = cacheData.firstReplySrc;
   	//CacheState state = cacheData.GetCacheState();

   	Address addr = BaseMemDevice::GetAddress(msg);
   	DebugAssertWithMessageID(pendingEviction.find(addr)==pendingEviction.end(), msg->MsgID());

   	if (msg->Type()==mt_Eviction)
   	{
   		const EvictionMsg* m = (const EvictionMsg*)msg;			
   		const BaseMsg*& firstReplyBase = cacheData.firstReply;
   		DebugAssertWithMessageID(pendingEviction.find(m->addr)==pendingEviction.end(), msg->MsgID());
   		DebugAssertWithMessageID(firstReplyBase==NULL, msg->MsgID());
   		DebugAssertWithMessageID(src==InvalidNodeID, msg->MsgID());

   		cacheData.SetCacheState(cs_WaitingForWritebackResponse);

   		// send writeback request to directory
   		WritebackRequestMsg* wrm = EM().CreateWritebackRequestMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeEvictionMsg(wrm, m);
   		SendMessageToDirectory(wrm, false);

   		pendingEviction[m->addr] = m;
   	} // if (msg->Type()==mt_Eviction)
   	else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(src==dirNode, m->solicitingMessage);
			
   		DebugAssertWithMessageID(pendingEviction.find(m->addr)==pendingEviction.end(), msg->MsgID());

   		if (m->requestingExclusive)
   		{
   			cacheData.SetCacheState(cs_Invalid);

   			// send exclusive response to the requester, which is the new owner
   			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeReadResponseMsg(rrm, m);
   			rrm->blockAttached = true;
   			rrm->exclusiveOwnership = true;
				rrm->interventionMessage = m->MsgID();
   			rrm->satisfied = true;
				SendMessageToRemoteCache(rrm, m->newOwner);

   			// send dirty transfer to directory
   			TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeEvictionMsg(tm, m);
   			tm->blockAttached = false;
   			tm->isDirty = true;
   			SendMessageToDirectory(tm, false);

   			//EM().DisposeMsg(msg);
   			ClearTempCacheData(cacheData);
   		} // if (firstReply->requestingExclusive)
   		else
   		{
   			cacheData.SetCacheState(cs_Shared);

   			// send shared response to the requester, which is the new owner
   			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeReadResponseMsg(rrm, m);
   			rrm->blockAttached = true;
   			rrm->exclusiveOwnership = false;
				rrm->interventionMessage = m->MsgID();
   			rrm->satisfied = true;
				SendMessageToRemoteCache(rrm, m->newOwner);

   			// send shared writeback to directory
   			WritebackMsg* wm = EM().CreateWritebackMsg(GetDeviceID(), m->GeneratingPC());
   			EM().InitializeEvictionMsg(wm, m);
   			wm->blockAttached = true;
   			wm->isShared = true;
   			SendMessageToDirectory(wm, false);

   			//EM().DisposeMsg(msg);
   			ClearTempCacheData(cacheData);
   		} // else !(if (m->requestingExclusive)
   	} // else if (msg->Type()==mt_Intervention)
		else
		{
			PrintError("OnCacheEvic", msg, "Unhandled message type");
		}
   } // OnCacheDirtyExclusive

	void OriginDirectory::OnCacheEviction(const EvictionMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheEviction",*m,"",src);
#endif
		cacheEvictionsReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
	}

	void OriginDirectory::OnCacheExclusive(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheEx",*msg,"",src);
#endif
		const BaseMsg*& firstReplyBase = cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;
		const EvictionMsg*& secondReply = cacheData.secondReply;

		if (msg->Type()==mt_Intervention)
		{
			DebugAssertWithMessageID(firstReplyBase==NULL, msg->MsgID());
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, msg->MsgID());

			const InterventionMsg* m = (const InterventionMsg*)msg;
			// message must be from directory
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(src==dirNode, m->MsgID());
			DebugAssertWithMessageID(m->newOwner!=InvalidNodeID, m->MsgID());

			if (m->requestingExclusive)
			{
				firstReplyBase = m;
				firstReplySrc = src;

				// send invalidate to cache
				InvalidateMsg* im = EM().CreateInvalidateMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeInvalidateMsg(im, m);
				DebugAssertWithMessageID(pendingRemoteInvalidates.find(im->MsgID())==pendingRemoteInvalidates.end(), im->MsgID());
				pendingRemoteInvalidates[im->MsgID()] = im;

				// set send time to 0 because we are saying that this
					// code in the OriginDirectory is already part of the cache
				SendMsg(localCacheConnectionID, EM().ReplicateMsg(im), localSendTime);
			} // if (m->requestingExclusive)
			else // !m->requestingExclusiv
			{
				firstReplyBase = m;
				firstReplySrc = src;

				// send ReadMsg to cache
				ReadMsg* rm = EM().CreateReadMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeReadMsg(rm, m);
				DebugAssertWithMessageID(pendingRemoteReads.find(rm->MsgID())==pendingRemoteReads.end(), rm->MsgID());
				pendingRemoteReads[rm->MsgID()] = rm;

				// set send time to 0 because we are saying that this
					// code in the OriginDirectory is already part of the cache
				SendMsg(localCacheConnectionID, EM().ReplicateMsg(rm), localSendTime);
			}
		} // if (msg->Type()==mt_Intervention)
		else if (msg->Type()==mt_Eviction)
		{
			const EvictionMsg* m = (const EvictionMsg*)msg;
			DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());

			if (firstReplyBase==NULL)
			{
				if (m->blockAttached)
				{
					OnCacheDirtyExclusive(m, src, cacheData);
				}
				else
				{
					//TODO!!!!!
					//OnCacheDirtyExclusive(m, src, cacheData);
					OnCacheCleanExclusive(m, src, cacheData);
				}
			}
			else
			{// if first reply is not null, then an intervention was received and processed
				// before the eviction message arrived, so we have to wait for a
				// response to the cache read/invalidate before proceeding
				DebugAssertWithMessageID(firstReplyBase->Type()==mt_Intervention, m->MsgID());

				secondReply = m;
				cacheData.SetCacheState(cs_ExclusiveInterventionEviction);
			}
		}
		else if (msg->Type()==mt_InvalidateResponse)
		{
			const InvalidateResponseMsg* m = (const InvalidateResponseMsg*)msg;
			DebugAssertWithMessageID(firstReplyBase!=NULL, m->solicitingMessage);
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(firstReplySrc==dirNode, m->solicitingMessage);
			DebugAssertWithMessageID(pendingRemoteInvalidates.find(m->solicitingMessage)!=pendingRemoteInvalidates.end(), m->MsgID());
			const InvalidateMsg*& im = pendingRemoteInvalidates[m->solicitingMessage];
			//EM().DisposeMsg(im);
			pendingRemoteInvalidates.erase(m->solicitingMessage);

			if (m->blockAttached)
			{
				OnCacheDirtyExclusive(firstReplyBase, firstReplySrc, cacheData);
			}
			else
			{
				OnCacheCleanExclusive(firstReplyBase, firstReplySrc, cacheData);
			}
		}
		else if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DebugAssertWithMessageID(firstReplyBase!=NULL, m->solicitingMessage);
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(firstReplySrc==dirNode, m->solicitingMessage);
			DebugAssertWithMessageID(pendingRemoteReads.find(m->solicitingMessage)!=pendingRemoteReads.end(), m->MsgID());
			const ReadMsg*& rm = pendingRemoteReads[m->solicitingMessage];
			//EM().DisposeMsg(rm);
			pendingRemoteReads.erase(m->solicitingMessage);

			if (m->isDirty)
			{
				OnCacheDirtyExclusive(firstReplyBase, firstReplySrc, cacheData);
			}
			else
			{
				OnCacheCleanExclusive(firstReplyBase, firstReplySrc, cacheData);
			}
		}
		else if (msg->Type()==mt_Read)
		{
			//TODO see if this is right 2011/01/27
			//ignore read messages when in exclusive
			const ReadMsg* m = (const ReadMsg*)msg;
			ReadResponseMsg* forward = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(forward, m);
			ProcessReadResponse(forward, cacheData);
		}
		else
		{
			PrintError("OnCacheEx", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheExclusiveInterventionEviction(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheExIntvEvic",*msg,"",src);
#endif
		const BaseMsg*& firstReplyBase = cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;
		const EvictionMsg*& secondReply = cacheData.secondReply;
		DebugAssertWithMessageID(secondReply!=NULL, secondReply->MsgID());

		if (msg->Type()==mt_InvalidateResponse)
		{
			const InvalidateResponseMsg* m = (const InvalidateResponseMsg*)msg;
			DebugAssertWithMessageID(pendingRemoteInvalidates.find(m->solicitingMessage)!=pendingRemoteInvalidates.end(), m->MsgID());
			const InvalidateMsg*& im = pendingRemoteInvalidates[m->solicitingMessage];
			//EM().DisposeMsg(im);
			pendingRemoteInvalidates.erase(m->solicitingMessage);

			// send response to cache
			EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(erm, secondReply);
			SendMsg(localCacheConnectionID, erm, localSendTime);

			DebugAssertWithMessageID(firstReplyBase->Type()==mt_Intervention, firstReplyBase->MsgID());
			const InterventionMsg* firstReply = (const InterventionMsg*)firstReplyBase;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(firstReplySrc==dirNode, firstReply->solicitingMessage);
			// the intervention has to be exclusive, because we received an InvalidateResponseMsg
			DebugAssertWithMessageID(firstReply->requestingExclusive, firstReply->solicitingMessage);
			
   		DebugAssertWithMessageID(pendingEviction.find(m->addr)==pendingEviction.end(), firstReply->solicitingMessage);
			
			// send exclusive read ack to the requester, which is the new owner
   		ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), firstReply->GeneratingPC());
   		EM().InitializeReadResponseMsg(rrm, firstReply);
			rrm->blockAttached = secondReply->blockAttached;
   		rrm->exclusiveOwnership = true;
			rrm->interventionMessage = firstReply->MsgID();
   		rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, firstReply->newOwner);

   		// send dirty transfer to directory
   		TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), firstReply->GeneratingPC());
   		EM().InitializeEvictionMsg(tm, firstReply);
   		tm->blockAttached = false;
   		tm->isDirty = true;
   		SendMessageToDirectory(tm, false);

			EM().DisposeMsg(secondReply);
   		ClearTempCacheData(cacheData);
			
			// should be set to invalid always regardless of the interventionMessage because
				// we received an eviction from the local cache
   		cacheData.SetCacheState(cs_Invalid);
		}
		else if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DebugAssertWithMessageID(pendingRemoteReads.find(m->solicitingMessage)!=pendingRemoteReads.end(), m->MsgID());
			const ReadMsg*& rm = pendingRemoteReads[m->solicitingMessage];
			//EM().DisposeMsg(rm);
			pendingRemoteReads.erase(m->solicitingMessage);

			// send response to cache
			EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(erm, secondReply);
			SendMsg(localCacheConnectionID, erm, localSendTime);

			DebugAssertWithMessageID(firstReplyBase->Type()==mt_Intervention, firstReplyBase->MsgID());
			const InterventionMsg* firstReply = (const InterventionMsg*)firstReplyBase;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(firstReplySrc==dirNode, firstReply->solicitingMessage);
			// the intervention has to be shared, because we received an ReadResponseMsg
			DebugAssertWithMessageID(!firstReply->requestingExclusive, firstReply->solicitingMessage);
			
   		DebugAssertWithMessageID(pendingEviction.find(m->addr)==pendingEviction.end(), firstReply->solicitingMessage);

   		// send shared response to the requester, which is the new owner
   		ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), firstReply->GeneratingPC());
   		EM().InitializeReadResponseMsg(rrm, firstReply);
			rrm->blockAttached = secondReply->blockAttached;
   		rrm->exclusiveOwnership = false;
			rrm->interventionMessage = firstReply->MsgID();
   		rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, firstReply->newOwner);

			if (secondReply->blockAttached)
			{
   			// send shared writeback to directory
   			WritebackMsg* wm = EM().CreateWritebackMsg(GetDeviceID(), firstReply->GeneratingPC());
   			EM().InitializeEvictionMsg(wm, firstReply);
   			wm->blockAttached = true;
   			wm->isShared = true;
   			SendMessageToDirectory(wm, false);
			}
			else
			{
				// send shared transfer to directory
				TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), firstReply->GeneratingPC());
				EM().InitializeEvictionMsg(tm, firstReply);
				tm->isShared = true;
				tm->isDirty = false;
				tm->blockAttached = false;
				SendMessageToDirectory(tm, false);
			}

   		//EM().DisposeMsg(msg);
			EM().DisposeMsg(secondReply);
   		ClearTempCacheData(cacheData);

			// should be set to invalid always regardless of the interventionMessage because
				// we received an eviction from the local cache			
   		cacheData.SetCacheState(cs_Invalid);
		} // else if (msg->Type()==mt_ReadResponse)
		else
		{
			PrintError("OnCacheExIntvEvic", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheExclusiveWaitingForSpeculativeReply(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheExWaitForSpecRep",*msg,"",src);
#endif
		DebugAssertWithMessageID(cacheData.firstReply!=NULL, msg->MsgID());
		DebugAssertWithMessageID(cacheData.firstReply->Type()==mt_ReadResponse, msg->MsgID());
		// this is okay, because we don't reassign cacheData.firstReply here
		const ReadResponseMsg* firstReply = (const ReadResponseMsg*)cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
		//CacheState state = cacheData.GetCacheState();

		if (msg->Type()==mt_SpeculativeReply)
		{
			const SpeculativeReplyMsg* m = (const SpeculativeReplyMsg*)msg;
			DebugAssertWithMessageID(m->solicitingMessage==firstReply->solicitingMessage, m->solicitingMessage);

			cacheData.SetCacheState(cs_Exclusive);

			// send read response to cache
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, m);
			rrm->blockAttached = true;
			rrm->exclusiveOwnership = true;
			// use 0 send time to simulate this as part of the cache
			SendMsg(localCacheConnectionID, rrm, localSendTime);

			ClearTempCacheData(cacheData);
			ProcessReadResponse(firstReply, cacheData);
		}
		/*
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			ProcessInterventionWhileInvalid(m, src);
		}
		*/
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		else
		{
			PrintError("OnCacheExWaitForSpecReply", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheIntervention(const InterventionMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheIntv",*m,"",src);
#endif
		cacheInterventionsReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
	}
	
	void OriginDirectory::OnCacheInvalid(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheInvalid",*msg,"",src);
#endif
		//CacheState state = cacheData.GetCacheState();

		if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			ProcessInterventionWhileInvalid(m, src);
		}// if (msgType==Intervention)
		else if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;

			if (m->requestingExclusive)
			{				
				DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());

				cacheData.SetCacheState(cs_WaitingForExclusiveReadResponse);

				// forward m to directory
				BaseMsg* forward = EM().ReplicateMsg(m);
				// do not dispose message, because pendingSharedRead or pendingExclusiveRead might have it
				SendMessageToDirectory(forward,false);
			} // if (m->requestingExclusive)
			else
			{ // is not requesting exclusive
				
				// must be from local
				DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());

				cacheData.SetCacheState(cs_WaitingForSharedReadResponse);

				// forward m to directory
				BaseMsg* forward = EM().ReplicateMsg(m);
				// do not dispose message, because pendingSharedRead or pendingExclusiveRead might have it
				SendMessageToDirectory(forward,false);
			} // else !m->requestingExclusive
		} // else if (msg->Type()==mt_Read)
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		/*
		 * this fix doesn't work
		else if (msg->Type()==mt_InvalidateResponse)
		{
			// ignore this message
			const InvalidateResponseMsg* m = (const InvalidateResponseMsg*)msg;
			EM().DisposeMsg(m);
		}
		*/
		else
		{
			PrintError("OnCacheInvalid",msg,"Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheShared(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheShared",*msg,"",src);
#endif
		//CacheState state = cacheData.GetCacheState();

		if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(src==dirNode, m->MsgID());
			const BaseMsg*& firstReply = cacheData.firstReply;
			DebugAssertWithMessageID(firstReply==NULL, msg->MsgID());
	   	NodeID& firstReplySrc = cacheData.firstReplySrc;
	   	DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, msg->MsgID());

			firstReply = msg;
			firstReplySrc = m->newOwner;
			const BaseMsg* forward = EM().ReplicateMsg(m);
			SendMsg(localCacheConnectionID, forward, localSendTime);
		}
		else if (msg->Type()==mt_InvalidateResponse)
		{
			DebugAssertWithMessageID(src==InvalidNodeID, msg->MsgID());
			DebugAssertWithMessageID(cacheData.firstReply!=NULL, msg->MsgID());
			DebugAssertWithMessageID(cacheData.firstReply->Type()==mt_Invalidate, msg->MsgID());
			// this is okay, because we don't assign firstReply to anything here
			const InvalidateMsg* firstReply = (const InvalidateMsg*)cacheData.firstReply;
	   	NodeID& firstReplySrc = cacheData.firstReplySrc;
	   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
	   	const InvalidateResponseMsg* m = (const InvalidateResponseMsg*)msg;
	   	DebugAssertWithMessageID(m->solicitingMessage==firstReply->MsgID(), firstReply->MsgID());

	   	cacheData.SetCacheState(cs_Invalid);

			// send invalidate ack to firstReplySrc
	   	InvalidateAckMsg* iam = EM().CreateInvalidateAckMsg(GetDeviceID(), m->GeneratingPC());
	   	EM().InitializeInvalidateResponseMsg(iam, m);
			DebugAssertWithMessageID(firstReply->newOwner!=InvalidNodeID, m->solicitingMessage);
			SendMessageToRemoteCache(iam, firstReply->newOwner);

	   	//EM().DisposeMsg(m);
	   	ClearTempCacheData(cacheData);
		}
		else if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;

			DebugAssertWithMessageID(m->requestingExclusive, m->MsgID());

			cacheData.SetCacheState(cs_WaitingForExclusiveReadResponse);

			// forward to directory
			SendMessageToDirectory(EM().ReplicateMsg(m));
		}
		else if (msg->Type()==mt_Eviction)
		{
			const EvictionMsg* m = (const EvictionMsg*)msg;

			cacheData.SetCacheState(cs_Invalid);

			// send Eviction Response Msg to cache
			EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingDeviceID());
			EM().InitializeEvictionResponseMsg(erm, m);
			SendMsg(localCacheConnectionID, erm, localSendTime);
			EM().DisposeMsg(m);
		}
		else
		{
			PrintError("OnCacheSh", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheSharedWaitingForSpeculativeReply(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheSharedWaitForSpecReply",*msg,"",src);
#endif
		DebugAssertWithMessageID(cacheData.firstReply!=NULL, msg->MsgID());
		DebugAssertWithMessageID(cacheData.firstReply->Type()==mt_ReadResponse, msg->MsgID());
		const ReadResponseMsg* firstReply = (const ReadResponseMsg*)cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
		//CacheState state = cacheData.GetCacheState();

		if (msg->Type()==mt_SpeculativeReply)
		{
			const SpeculativeReplyMsg* m = (const SpeculativeReplyMsg*)msg;
			DebugAssertWithMessageID(m->solicitingMessage==firstReply->solicitingMessage, m->solicitingMessage);

			cacheData.SetCacheState(cs_Shared);

			// send read response to cache
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, m);
			rrm->blockAttached = true;
			rrm->exclusiveOwnership = false;
			// use 0 send time to simulate this as part of the cache
			SendMsg(localCacheConnectionID, rrm, localSendTime);

			ClearTempCacheData(cacheData);
			ProcessReadResponse(firstReply, cacheData);
		}
		/*
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			ProcessInterventionWhileInvalid(m, src);
		}
		*/
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		else
		{
			PrintError("OnCacheShWaitForSpecReply", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheSpeculativeReply(const SpeculativeReplyMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheSpecReply",*m,"",src);
#endif
		cacheSpeculativeRepliesReceived++;
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
	}

	void OriginDirectory::OnCacheWaitingForExclusiveReadResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForExReadRes",*msg,"",src);
#endif
		const BaseMsg*& firstReplyBase = cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;
		int& invalidAcksReceived = cacheData.invalidAcksReceived;
		//const ReadMsg*& pendingExclusiveRead = cacheData.pendingExclusiveRead;
		DebugAssertWithMessageID(cacheData.cacheDataPendingLocalReads.size()>0, firstReplyBase->MsgID());
		const ReadMsg*& pendingRead = cacheData.cacheDataPendingLocalReads.front();
		bool& isCanceledReadRequest = cacheData.isCanceledReadRequest;

		if (msg->Type()==mt_CacheNak)
		{
			const CacheNakMsg* m = (const CacheNakMsg*)msg;
			DebugAssertWithMessageID(pendingRead!=NULL, m->solicitingMessage);
			DebugAssertWithMessageID(m->solicitingMessage==pendingRead->MsgID(), m->solicitingMessage);

			if (firstReplyBase==NULL)
			{
				DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);

				if (m->interventionMessage==0)
				{
					// resend request
					ReadMsg* forward  = (ReadMsg*)EM().ReplicateMsg(pendingRead);
					ResendRequestToDirectory(forward);
					EM().DisposeMsg(m);
				}
				else
				{
					// if CacheNak comes from an intervention, then it means that a SpecReply from
						// the directory was also sent, but we are still in WaitingForExReadRes state,
						// which means that the SpeculativeReply has not arrived, yet, so we
						// have to wait for the SpeculativeReply to arrive before we can resend
						// the read request
					firstReplyBase = m;
				}
			}
			else
			{
				// if firstReplyBase is not NULL, it means that an invalidate has arrived into the cache,
					// and we are waiting for the invalidate to complete before resending the read request
				DebugAssertWithMessageID(firstReplyBase->Type()==mt_Invalidate, m->solicitingMessage);
				DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, m->solicitingMessage);
				DebugAssertWithMessageID(!isCanceledReadRequest, m->solicitingMessage);
				isCanceledReadRequest = true;
			}
		}
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;

			DebugAssertWithMessageID(firstReplyBase==NULL, msg->MsgID());
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, msg->MsgID());
			DebugAssertWithMessageID(src!=InvalidNodeID, msg->MsgID());

			firstReplyBase = m;
			firstReplySrc = src;

			const InvalidateMsg* forward = (const InvalidateMsg*)EM().ReplicateMsg(m);

			SendMsg(localCacheConnectionID, forward, localSendTime);
		}
		else if (msg->Type()==mt_InvalidateResponse)
		{
			DebugAssertWithMessageID(firstReplyBase!=NULL, msg->MsgID());
			DebugAssertWithMessageID(firstReplyBase->Type()==mt_Invalidate, msg->MsgID());
			const InvalidateMsg* firstReply = (const InvalidateMsg*)firstReplyBase;
			DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
			DebugAssertWithMessageID(src==InvalidNodeID, msg->MsgID());
			const InvalidateResponseMsg* m = (const InvalidateResponseMsg*) msg;
			DebugAssertWithMessageID(m->solicitingMessage==firstReplyBase->MsgID(), firstReplyBase->MsgID());

			// send invalidate ack to requester
			InvalidateAckMsg* iam = EM().CreateInvalidateAckMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeInvalidateResponseMsg(iam, m);
			DebugAssertWithMessageID(firstReply->newOwner!=InvalidNodeID, m->solicitingMessage);
			SendMessageToRemoteCache(iam, firstReply->newOwner);

			//EM().DisposeMsg(m);
			ClearTempCacheData(cacheData);

			// if a read request has been canceled because we were processing an invalidate
				// resend the read request
			if (isCanceledReadRequest)
			{
				isCanceledReadRequest = false;
				ReadMsg* forward = (ReadMsg*)EM().ReplicateMsg(pendingRead);
				ResendRequestToDirectory(forward);
			}
		}
		else if (msg->Type()==mt_ReadReply)
		{
			const ReadReplyMsg* m = (const ReadReplyMsg*)msg;
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);
			DebugAssertWithMessageID(src!=InvalidNodeID, m->solicitingMessage);

			// has to be exclusively owned
			DebugAssertWithMessageID(m->exclusiveOwnership, m->solicitingMessage);
			
			// pendingInvalidates has to be greater than or equal to 0
			DebugAssertWithMessageID(m->pendingInvalidates>=0, m->solicitingMessage);

			if (m->pendingInvalidates==0)
			{
				cacheData.SetCacheState(cs_Exclusive);
				SendMessageToLocalCache(m);
				// shouldn't clear temp data here
				ProcessReadResponse(m, cacheData);
			}
			else if (m->pendingInvalidates > 0)
			{
				cacheData.SetCacheState(cs_WaitingForKInvalidatesJInvalidatesReceived);
				firstReplyBase = m;
				firstReplySrc = src;
			}
		} // else if (msg->Type()==mt_ReadReply)
		else if (msg->Type()==mt_InvalidateAck)
		{
			const InvalidateAckMsg* m = (const InvalidateAckMsg*)msg;

			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);
			// don't put anything in firstReplyBase because that's to be used by Exclusive Reply
			DebugAssertWithMessageID(invalidAcksReceived==InvalidInvalidAcksReceived, m->solicitingMessage);

			invalidAcksReceived++;
			cacheData.SetCacheState(cs_WaitingForKInvalidatesJInvalidatesReceived);

			//EM().DisposeMsg(msg);
		}
		else if (msg->Type()==mt_SpeculativeReply)
		{
			const SpeculativeReplyMsg* m = (const SpeculativeReplyMsg*)msg;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(src==dirNode, m->solicitingMessage);

			if (firstReplyBase==NULL)
			{
				cacheData.SetCacheState(cs_WaitingForExclusiveResponseAck);

				firstReplyBase = m;
				firstReplySrc = src;
			}
			else
			{// if we received a cache nak, discard the speculativeReply and resend request
				DebugAssertWithMessageID(firstReplyBase->Type()==mt_CacheNak, m->solicitingMessage);
				const CacheNakMsg* firstReply = (const CacheNakMsg*)firstReplyBase;
				
				// resend read request to directory
				ReadMsg* forward  = (ReadMsg*)EM().ReplicateMsg(pendingRead);
				ResendRequestToDirectory(forward);
				EM().DisposeMsg(m);
				ClearTempCacheData(cacheData);
			}
		}
		else if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DebugAssertWithMessageID(src!=InvalidNodeID, m->solicitingMessage);
			DebugAssertWithMessageID(m->exclusiveOwnership==true, m->solicitingMessage);

			cacheData.SetCacheState(cs_ExclusiveWaitingForSpeculativeReply);
			firstReplyBase = m;
			firstReplySrc = src;
		}
		// don't know if I should enable this or not
		/*
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			// perhaps we can use ProcessInterventionWhileInvalid instead of
				// SendInterventionNaks because this is going to be exclusive?
				// but perhaps it's not a good idea because naks are usually more robust
			//ProcessInterventionWhileInvalid(m, src);
			SendInterventionNaks(m);
		}
		*/
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);
			//ProcessInterventionWhileInvalid(m, src);
			SendInterventionNaks(m);
		}
		else
		{
			PrintError("OnCacheWaitForExReadRes", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheWaitingForExclusiveResponseAck(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForExReadAck",*msg,"",src);
#endif
   	const BaseMsg*& firstReply = cacheData.firstReply;
   	DebugAssertWithMessageID(firstReply!=NULL, msg->MsgID());
   	NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
		//CacheState state = cacheData.GetCacheState();

		if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DebugAssertWithMessageID(m->exclusiveOwnership==true, m->MsgID());

			cacheData.SetCacheState(cs_Exclusive);

			// send read response to cache
			ReadResponseMsg* rrm = (ReadResponseMsg*)EM().ReplicateMsg(m);
			rrm->blockAttached = true;
			rrm->exclusiveOwnership = true;
			// use 0 for localSendTime to simulate this device as part of the cache
			SendMsg(localCacheConnectionID, rrm, localSendTime);

			//EM().DisposeMsg(m);
			ClearTempCacheData(cacheData);
			ProcessReadResponse(m, cacheData);
		}
		else if (msg->Type()==mt_CacheNak)
		{
			const CacheNakMsg* m = (const CacheNakMsg*)msg;
			DebugAssertWithMessageID(cacheData.cacheDataPendingLocalReads.size()>0, m->solicitingMessage);
			const ReadMsg* ref = cacheData.cacheDataPendingLocalReads.front();
			DebugAssertWithMessageID(ref->MsgID()==m->solicitingMessage, m->solicitingMessage);

			// resend read request
			ReadMsg* forward = (ReadMsg*)EM().ReplicateMsg(ref);
			ResendRequestToDirectory(forward);

			ClearTempCacheData(cacheData);

			cacheData.SetCacheState(cs_WaitingForExclusiveReadResponse);
		}
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			SendInterventionNaks(m);
		}
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		else
		{
			PrintError("OnCacheWaitForExResAck", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheWaitingForIntervention(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForIntv",*msg,"",src);
#endif
   	DebugAssertWithMessageID(cacheData.firstReply!=NULL, msg->MsgID());
   	DebugAssertWithMessageID(cacheData.firstReply->Type()==mt_WritebackAck, msg->MsgID());
   	const WritebackAckMsg* firstReply = (const WritebackAckMsg*)cacheData.firstReply;
		DebugAssertWithMessageID(firstReply->isBusy==true, msg->MsgID());
   	NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
   	//CacheState state = cacheData.GetCacheState();

   	if (msg->Type()==mt_Intervention)
   	{
   		const InterventionMsg* m = (const InterventionMsg*)msg;
			DebugAssertWithMessageID(m->newOwner!=InvalidNodeID, m->solicitingMessage);

			DebugAssertWithMessageID(pendingEviction.find(firstReply->addr)!=pendingEviction.end(), firstReply->solicitingMessage);
			//EM().DisposeMsg(pendingEviction[firstReply->addr]);
			pendingEviction.erase(firstReply->addr);

			cacheData.SetCacheState(cs_Invalid);

			// send writeback response to cache
			EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(erm, m);
			SendMsg(localCacheConnectionID, erm, localSendTime);

			// discard intervention without using it
			//EM().DisposeMsg(m);
			ClearTempCacheData(cacheData);

			// process more pendingLocalReads if there are any
			ProcessRemainingPendingLocalReads(cacheData);
   	}
   	else
   	{
   		PrintError("OnCacheWaitForIntv", msg, "Unhandled message type");
   	}
	}

	void OriginDirectory::OnCacheWaitingForKInvalidatesJInvalidatesReceived(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForKIJIR",*msg,"",src);
#endif
		//CacheState state = cacheData.GetCacheState();
		int& invalidateAcksReceived = cacheData.invalidAcksReceived;
		const BaseMsg*& firstReplyBase = cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;

		if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* im = (const InterventionMsg*)msg;
			SendInterventionNaks(im);
		}
		else if (msg->Type()==mt_InvalidateAck)
		{
			if (cacheData.firstReply!=NULL)
			{
				DebugAssertWithMessageID(firstReplyBase->Type()==mt_ReadReply, msg->MsgID());
				const ReadReplyMsg* firstReply = (const ReadReplyMsg*) firstReplyBase;
				DebugAssertWithMessageID(firstReply->exclusiveOwnership, firstReply->solicitingMessage);
				DebugAssertWithMessageID(firstReply->pendingInvalidates > invalidateAcksReceived, firstReply->solicitingMessage);

				invalidateAcksReceived++;
				if (firstReply->pendingInvalidates==invalidateAcksReceived)
				{
					cacheData.SetCacheState(cs_Exclusive);
					SendMessageToLocalCache(firstReply);
					ClearTempCacheData(cacheData);
					ProcessReadResponse(firstReply, cacheData);
				}
			} // if (cacheData.firstReply!=NULL)
			else
			{
				// haven't received exclusive reply with pending invalidates
				invalidateAcksReceived++;
				//EM().DisposeMsg(msg);
			} // else cacheData.firstReply==NULL
		}
		else if (msg->Type()==mt_ReadReply)
		{
			const ReadReplyMsg* m = (const ReadReplyMsg*)msg;
			DebugAssertWithMessageID(m->exclusiveOwnership, m->solicitingMessage);
			DebugAssertWithMessageID(m->pendingInvalidates>0, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);

			if (m->pendingInvalidates==invalidateAcksReceived)
			{
				cacheData.SetCacheState(cs_Exclusive);
				//EM().DisposeMsg(m);
				SendMessageToLocalCache(m);
				ClearTempCacheData(cacheData);
				ProcessReadResponse(m, cacheData);
			}
			else
			{
				cacheData.firstReply = m;
				firstReplySrc = src;
			}
		}
		/*
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		*/
		else
		{
			PrintError("OnCacheWaitForKInvalidatesJInvalidatesReceived", msg, "Unhandled message type");
		}
	} // OnCacheWaitingForKInvalidatesJInvalidatesReceived

	void OriginDirectory::OnCacheWaitingForSharedReadResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForShReadRes",*msg,"",src);
#endif
		//CacheState state = cacheData.GetCacheState();
		const BaseMsg*& firstReplyBase = cacheData.firstReply;
		NodeID& firstReplySrc = cacheData.firstReplySrc;
		//const ReadMsg*& pendingSharedRead = cacheData.pendingSharedRead;
		DebugAssertWithMessageID(cacheData.cacheDataPendingLocalReads.size()>0, msg->MsgID());
		const ReadMsg*& pendingRead = cacheData.cacheDataPendingLocalReads.front();

		if (msg->Type()==mt_CacheNak)
		{
			const CacheNakMsg* m = (const CacheNakMsg*)msg;
			DebugAssertWithMessageID(m->solicitingMessage==pendingRead->MsgID(), m->MsgID());
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);

			if (m->interventionMessage==0)
			{
				// resend request
				ReadMsg* forward  = (ReadMsg*)EM().ReplicateMsg(pendingRead);
				ResendRequestToDirectory(forward);
				EM().DisposeMsg(m);
			}
			else
			{
				// if CacheNak comes from an intervention, then it means that a SpecReply from
					// the directory was also sent, but we are still in WaitingForShReadRes state,
					// which means that the SpeculativeReply has not arrived, yet, so we
					// have to wait for the SpeculativeReply to arrive before we can resend
					// the read request
				firstReplyBase = m;
			}
		}
		else if (msg->Type()==mt_ReadReply)
		{
			const ReadReplyMsg* m = (const ReadReplyMsg*)msg;
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);

			if (m->exclusiveOwnership)
			{
				cacheData.SetCacheState(cs_Exclusive);
				SendMessageToLocalCache(m);
				// don't clear temp data here since there shouldn't be any temp data
				ProcessReadResponse(m, cacheData);
			}
			else
			{
				cacheData.SetCacheState(cs_Shared);
				SendMessageToLocalCache(m);
				// don't clear temp data here since there shouldn't be any temp data
				ProcessReadResponse(m, cacheData);
			}
		}
		else if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DebugAssertWithMessageID(m->exclusiveOwnership==false, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);

			cacheData.SetCacheState(cs_SharedWaitingForSpeculativeReply);
			firstReplyBase = m;
			firstReplySrc = src;
		}
		else if (msg->Type()==mt_SpeculativeReply)
		{
			const SpeculativeReplyMsg* m = (const SpeculativeReplyMsg*)msg;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
			DebugAssertWithMessageID(src==dirNode, m->solicitingMessage);

			if (firstReplyBase==NULL)
			{
				cacheData.SetCacheState(cs_WaitingForSharedResponseAck);

				firstReplyBase = m;
				firstReplySrc = src;
			}
			else
			{// if we received a cache nak, discard the speculativeReply and resend request
				DebugAssertWithMessageID(firstReplyBase->Type()==mt_CacheNak, m->solicitingMessage);
				const CacheNakMsg* firstReply = (const CacheNakMsg*)firstReplyBase;
				
				// resend read request to directory
				ReadMsg* forward  = (ReadMsg*)EM().ReplicateMsg(pendingRead);
				ResendRequestToDirectory(forward);
				EM().DisposeMsg(m);
				ClearTempCacheData(cacheData);
			}
		}
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			DebugAssertWithMessageID(firstReplyBase==NULL, m->solicitingMessage);
			DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, m->solicitingMessage);
			//ProcessInterventionWhileInvalid(m, src);
			SendInterventionNaks(m);
		}
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		else
		{
			PrintError("OnCacheWaitForShReadRes", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheWaitingForWritebackBusyAck(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForWbBusyAck",*msg,"",src);
#endif
   	DebugAssertWithMessageID(cacheData.firstReply!=NULL, msg->MsgID());
   	DebugAssertWithMessageID(cacheData.firstReply->Type()==mt_Intervention, msg->MsgID());
   	NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
   	//CacheState state = cacheData.GetCacheState();

   	if (msg->Type()==mt_WritebackAck)
   	{
   		const WritebackAckMsg* m = (const WritebackAckMsg*)msg;

			// it has to be a writeback busy ack
			DebugAssertWithMessageID(m->isBusy && !m->isExclusive, m->solicitingMessage);
			
			DebugAssertWithMessageID(pendingEviction.find(m->addr)!=pendingEviction.end(), m->solicitingMessage);
			//EM().DisposeMsg(pendingEviction[m->addr]);
			pendingEviction.erase(m->addr);

   		cacheData.SetCacheState(cs_Invalid);

			// send writeback response to cache
			EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(erm, m);
			SendMsg(localCacheConnectionID, erm, localSendTime);

   		//EM().DisposeMsg(m);
   		ClearTempCacheData(cacheData);
				
			ProcessRemainingPendingLocalReads(cacheData);
   	}
   	else
   	{
   		PrintError("OnCacheWaitForWbBusyAck", msg, "Unhandled message type");
   	}
	}

	void OriginDirectory::OnCacheWaitingForWritebackResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForWbRes",*msg,"",src);
#endif
   	const BaseMsg*& firstReply = cacheData.firstReply;
   	DebugAssertWithMessageID(firstReply==NULL, msg->MsgID());
   	NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc==InvalidNodeID, msg->MsgID());
		//CacheState state = cacheData.GetCacheState();

		Address addr = BaseMemDevice::GetAddress(msg);
		DebugAssertWithMessageID(pendingEviction.find(addr)!=pendingEviction.end(), msg->MsgID());

		if (msg->Type()==mt_WritebackAck)
		{
			const WritebackAckMsg* m = (const WritebackAckMsg*)msg;
			// the message should only have one of these flags enabled
			DebugAssertWithMessageID( (m->isBusy&&!m->isExclusive) || (!m->isBusy&&m->isExclusive), m->solicitingMessage);

			if (m->isExclusive)
			{
				cacheData.SetCacheState(cs_Invalid);

				// send writeback response to cache
				EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeEvictionResponseMsg(erm, m);
				SendMsg(localCacheConnectionID, erm, localSendTime);

				//EM().DisposeMsg(m);
				//EM().DisposeMsg(pendingEviction[m->addr]);
				pendingEviction.erase(m->addr);

				ProcessRemainingPendingLocalReads(cacheData);		
			}
			else if (m->isBusy)
			{
				cacheData.SetCacheState(cs_WaitingForIntervention);

				firstReply = m;
				firstReplySrc = src;
			}
		} // if (msg->Type()==mt_WritebackAck)
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			DebugAssertWithMessageID(m->newOwner!=InvalidNodeID, m->solicitingMessage);

			cacheData.SetCacheState(cs_WaitingForWritebackBusyAck);

			firstReply = msg;
			firstReplySrc = m->newOwner;
		}
		else
		{
			PrintError("OnCacheWaitForWbRes", msg, "Unhandled message type");
		}
	} // OnCacheWaitingForWritebackResponse

		/**
	 * Received a readResponse from the directory for the cache
	 */
	void OriginDirectory::OnCacheWaitingForSharedResponseAck(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWaitForShReadAck",*msg,"",src);
#endif
   	const BaseMsg*& firstReply = cacheData.firstReply;
   	DebugAssertWithMessageID(firstReply!=NULL, msg->MsgID());
   	NodeID& firstReplySrc = cacheData.firstReplySrc;
   	DebugAssertWithMessageID(firstReplySrc!=InvalidNodeID, msg->MsgID());
		//CacheState state = cacheData.GetCacheState();

		if (msg->Type()==mt_ReadResponse)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			DebugAssertWithMessageID(m->exclusiveOwnership==false, m->MsgID());

			cacheData.SetCacheState(cs_Shared);

			// send read response to cache
			ReadResponseMsg* rrm = (ReadResponseMsg*)EM().ReplicateMsg(m);
			rrm->blockAttached = true;
			rrm->exclusiveOwnership = false;
			// use 0 to simulate this device as part of the cache
			SendMsg(localCacheConnectionID, rrm, localSendTime);

			//EM().DisposeMsg(m);
			ClearTempCacheData(cacheData);
			ProcessReadResponse(m, cacheData);
		}
		else if (msg->Type()==mt_CacheNak)
		{
			const CacheNakMsg* m = (const CacheNakMsg*)msg;
			DebugAssertWithMessageID(cacheData.cacheDataPendingLocalReads.size()>0, m->solicitingMessage);
			const ReadMsg* ref = cacheData.cacheDataPendingLocalReads.front();
			DebugAssertWithMessageID(ref->MsgID()==m->solicitingMessage, m->solicitingMessage);

			// resend read request
			ReadMsg* forward = (ReadMsg*)EM().ReplicateMsg(ref);
			ResendRequestToDirectory(forward);

			ClearTempCacheData(cacheData);

			cacheData.SetCacheState(cs_WaitingForSharedReadResponse);
		}
		else if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			SendInterventionNaks(m);
		}
		else if (msg->Type()==mt_Invalidate)
		{
			const InvalidateMsg* m = (const InvalidateMsg*)msg;
			ProcessInvalidateWhileInvalid(m, src);
		}
		else
		{
			PrintError("OnCacheWaitForShResAck", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnCacheWritebackAck(const WritebackAckMsg* m, NodeID src)
   {
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("CacheWbAck",*m,"",src);
#endif
		CacheData& cacheData = GetCacheData(m->addr);
		OnCache(m, src, cacheData);
		return;
	}

	void OriginDirectory::OnDirectory(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		Address addr = BaseMemDevice::GetAddress(msg);
		DirectoryState& state = directoryData.state;
		directoryData.previousMessage = directoryData.currentMessage;
		directoryData.currentMessage = msg;

		switch(state)
		{
		case ds_BusyExclusiveMemoryAccess:
			OnDirectoryBusyExclusiveMemoryAccess(msg, src, directoryData, isFromMemory);
			break;
		case ds_BusyExclusive:
			OnDirectoryBusyExclusive(msg, src, directoryData, isFromMemory);
			break;
		case ds_BusyExclusiveMemoryAccessWritebackRequest:
			OnDirectoryBusyExclusiveMemoryAccessWritebackRequest(msg, src, directoryData, isFromMemory);
			break;
		case ds_BusyShared:
			OnDirectoryBusyShared(msg, src, directoryData, isFromMemory);
			break;
		case ds_BusySharedMemoryAccess:
			OnDirectoryBusySharedMemoryAccess(msg, src, directoryData, isFromMemory);
			break;
		case ds_BusySharedMemoryAccessWritebackRequest:
			OnDirectoryBusySharedMemoryAccessWritebackRequest(msg, src, directoryData, isFromMemory);
			break;
		case ds_ExclusiveMemoryAccess:
			OnDirectoryExclusiveMemoryAccess(msg, src, directoryData, isFromMemory);
			break;
		case ds_Exclusive:
			OnDirectoryExclusive(msg, src, directoryData, isFromMemory);
			break;
		case ds_Shared:
			OnDirectoryShared(msg, src, directoryData, isFromMemory);
			break;
		case ds_SharedExclusiveMemoryAccess:
			OnDirectorySharedExclusiveMemoryAccess(msg, src, directoryData, isFromMemory);
			break;
		case ds_SharedMemoryAccess:
			OnDirectorySharedMemoryAccess(msg, src, directoryData, isFromMemory);
			break;
		case ds_Unowned:
			OnDirectoryUnowned(msg, src, directoryData, isFromMemory);
			break;
		default:
			PrintError("OnDir",msg, "Unimplemented directory state");
			break;
		}
	}

	void OriginDirectory::OnDirectoryBusyExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirBusyEx",*msg,"",src);
#endif
		DirectoryState& state = directoryData.state;
		const ReadMsg*& firstRequest = directoryData.firstRequest;
		DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
		NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	NodeID& owner = directoryData.owner;
   	NodeID& previousOwner = directoryData.previousOwner;
		DebugAssertWithMessageID(previousOwner!=InvalidNodeID, msg->MsgID());
   	HashSet<NodeID>& sharers =  directoryData.sharers;
   	DebugAssertWithMessageID(sharers.size()==0, msg->MsgID());

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;

			// send nak to requester
			SendCacheNak(m, src);
		}
		else if (msg->Type()==mt_WritebackRequest)
		{
			const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());

			state = ds_Exclusive;

			// send exclusive response to owner
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, firstRequest);
			rrm->blockAttached = true;
			rrm->exclusiveOwnership = true;
			rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, owner);

			// send writeback busy ack to requester
			WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(wam, m);
			wam->isBusy = true;
			SendMessageToRemoteCache(wam, src);

			// send memory write
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);

			//EM().DisposeMsg(firstRequest);
			ClearTempDirectoryData(directoryData);
		}
		else if (msg->Type()==mt_Transfer)
		{
			const TransferMsg* m = (const TransferMsg*)msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());
			DebugAssertWithMessageID(m->isDirty, m->MsgID());

			state = ds_Exclusive;

			//EM().DisposeMsg(firstRequest);
			ClearTempDirectoryData(directoryData);
		}
		else if (msg->Type()==mt_DirectoryNak)
		{
			const DirectoryNakMsg* m = (const DirectoryNakMsg*)msg;
			state = ds_Exclusive;

			owner = previousOwner;

			//EM().DisposeMsg(firstRequest);
			//EM().DisposeMsg(m);
			ClearTempDirectoryData(directoryData);
		}
		else
		{
			PrintError("OnDirBusyEx", msg, "Unhandled message type");
		}

		// dispose message
		//EM().DisposeMsg(msg);
	} // OnDirectoryBusyExclusive

   void OriginDirectory::OnDirectoryBusyExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirBusyExMemAcc",*msg,"",src);
#endif
   	DirectoryState& state = directoryData.state;
   	const ReadMsg*& firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	const WritebackRequestMsg*& secondRequest = directoryData.secondRequest;
   	DebugAssertWithMessageID(secondRequest==NULL, msg->MsgID());
   	NodeID& secondRequestSrc = directoryData.secondRequestSrc;
   	DebugAssertWithMessageID(secondRequestSrc==InvalidNodeID, msg->MsgID());
   	NodeID& previousOwner = directoryData.previousOwner;
   	DebugAssertWithMessageID(previousOwner!=InvalidNodeID, msg->MsgID());

   	if (msg->Type()==mt_WritebackRequest)
   	{
   		const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;

   		state = ds_BusyExclusiveMemoryAccessWritebackRequest;

   		secondRequest = m;
   		secondRequestSrc = src;
   	}
   	else if (msg->Type()==mt_Read)
   	{
   		const ReadMsg* m = (const ReadMsg*)msg;
   		SendCacheNak(m, src);
   	}
   	else if (isFromMemory)
   	{
   		DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
   		const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
   		PerformMemoryReadResponseCheck(m, src);

   		state = ds_BusyExclusive;

   		// send intervention exclusive request to previous owner
   		InterventionMsg* im = EM().CreateInterventionMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeReadMsg(im, m);
   		// intervention messages always fetch block
   		im->alreadyHasBlock = false;
			im->newOwner = firstRequestSrc;
   		im->requestingExclusive = true;
			im->solicitingMessage = firstRequest->MsgID();
   		SendMessageToRemoteCache(im, previousOwner);

   		// send speculative reply to requester
   		SpeculativeReplyMsg* srm = EM().CreateSpeculativeReplyMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeReadResponseMsg(srm, firstRequest);
   		srm->blockAttached = true;
   		srm->satisfied = true;
   		SendMessageToRemoteCache(srm, firstRequestSrc);
   	} // else if (isFromMemory)
   	else
   	{
   		PrintError("OnDirBusyExMemAcc", msg, "Unhandled message type");
   	}
	}

   void OriginDirectory::OnDirectoryBusyExclusiveMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirBusyExMemAccWbReq",*msg,"",src);
#endif
   	DirectoryState& state = directoryData.state;
   	const ReadMsg*& firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	const WritebackRequestMsg*& secondRequest = directoryData.secondRequest;
   	DebugAssertWithMessageID(secondRequest!=NULL, msg->MsgID());
   	NodeID& secondRequestSrc = directoryData.secondRequestSrc;
   	DebugAssertWithMessageID(secondRequestSrc!=InvalidNodeID, msg->MsgID());
   	NodeID& owner = directoryData.owner;

   	if (msg->Type()==mt_Read)
   	{
   		// send nak to requester
   		const ReadMsg* m = (const ReadMsg*) msg;
   		SendCacheNak(m, src);
   	}
   	else if (isFromMemory)
   	{
   		DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
   		const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
   		PerformMemoryReadResponseCheck(m, src);
   		DebugAssertWithMessageID(m->solicitingMessage==firstRequest->MsgID(), m->solicitingMessage);

   		state = ds_Exclusive;

   		// send exclusive reply to owner
   		ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeReadResponseMsg(rrm, firstRequest);
   		rrm->blockAttached = true;
   		rrm->exclusiveOwnership = true;
   		rrm->satisfied = true;
   		SendMessageToRemoteCache(rrm, owner);

   		// send writeback exclusive ack to requester
   		WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
   		// soliciting message id should be the same as the WritebackMsg's id
   		EM().InitializeEvictionResponseMsg(wam, secondRequest);
   		wam->isExclusive = true;
   		SendMessageToRemoteCache(wam, secondRequestSrc);

   		// send memory write
   		WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeWriteMsg(wm, m);
   		SendRequestToMemory(wm);

			//EM().DisposeMsg(secondRequest);
   		ClearTempDirectoryData(directoryData);
   	} // else if (isFromMemory)
   	else
   	{
   		PrintError("OnDirBusyExMemAccWbReq", msg, "Unhandled message type");
   	}
	}// OnDirectoryBusyExclusiveMemoryAccessWritebackRequest

   void OriginDirectory::OnDirectoryBusyShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirBusySh",*msg,"",src);
#endif
		DirectoryState& state = directoryData.state;
		const ReadMsg*& firstRequest = directoryData.firstRequest;
		DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
		NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	NodeID& owner = directoryData.owner;
   	NodeID& previousOwner = directoryData.previousOwner;
   	DebugAssertWithMessageID(previousOwner!=InvalidNodeID, msg->MsgID());
   	HashSet<NodeID>& sharers =  directoryData.sharers;

		if (msg->Type()==mt_Writeback)
		{
			const WritebackMsg* m = (const WritebackMsg*) msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());
			state = ds_Shared;

			// write to memory
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);

			////EM().DisposeMsg(firstRequest);
			ClearTempDirectoryData(directoryData);
		}
		else if (msg->Type()==mt_Transfer)
		{
			const TransferMsg* m = (const TransferMsg*)msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());
			state = ds_Shared;
			ClearTempDirectoryData(directoryData);
		}
		else if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*) msg;

			// send cache nak to requester
			SendCacheNak(m, src);
		}
		else if (msg->Type()==mt_WritebackRequest)
		{
			const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());

			state = ds_Shared;

			// send shared response to owner
			DebugAssertWithMessageID(owner!=InvalidNodeID, m->MsgID());
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, firstRequest);
			rrm->exclusiveOwnership = false;
			rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, owner);

			// send writeback busy ack to requester
			WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(wam, m);
			wam->isBusy = true;
			SendMessageToRemoteCache(wam, src);

			// memory write
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);

			//EM().DisposeMsg(firstRequest);
			ClearTempDirectoryData(directoryData);
		}//else if (msg->Type()==mt_WritebackRequest)
		else if (msg->Type()==mt_DirectoryNak)
		{
			const DirectoryNakMsg* m = (const DirectoryNakMsg*)msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());
			state = ds_Exclusive;
			owner = previousOwner;
			sharers.clear();

			//EM().DisposeMsg(firstRequest);
			ClearTempDirectoryData(directoryData);
		} // else if (msg->Type()==mt_DirectoryNak)
		else
		{
			PrintError("OnDirBusySh", msg, "Unexpected message type");
		}

		// dispose message
		//EM().DisposeMsg(msg);
	}

   void OriginDirectory::OnDirectoryBusySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirBusyShMemAcc",*msg,"",src);
#endif
   	const ReadMsg*& firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	DirectoryState& state = directoryData.state;
   	NodeID& previousOwner = directoryData.previousOwner;
   	const WritebackRequestMsg*& secondRequest = directoryData.secondRequest;
   	DebugAssertWithMessageID(secondRequest==NULL, msg->MsgID());
   	NodeID& secondRequestSrc = directoryData.secondRequestSrc;
   	DebugAssertWithMessageID(secondRequestSrc==InvalidNodeID, msg->MsgID());

		if (isFromMemory)
		{
			DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
			const ReadResponseMsg* m = (const ReadResponseMsg*) msg;
			PerformMemoryReadResponseCheck(m, src);

			state = ds_BusyShared;

			// send intervention shared request to previous owner
			DebugAssertWithMessageID(previousOwner!=InvalidNodeID, m->MsgID());
			InterventionMsg* im = EM().CreateInterventionMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadMsg(im, m);
			// set this to false in order to always fetch a block, since
				// intervention messages always fetch the block
			im->alreadyHasBlock = false;
			im->newOwner = firstRequestSrc;
			im->requestingExclusive = false;
			im->solicitingMessage = firstRequest->MsgID();
			SendMessageToRemoteCache(im, previousOwner);

			// send speculative reply to requester
			SpeculativeReplyMsg* srm = EM().CreateSpeculativeReplyMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(srm, firstRequest);
			srm->blockAttached = true;
			srm->satisfied = true;
			SendMessageToRemoteCache(srm, firstRequestSrc);
		} // if (isFromMemory)
		else if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;

			// send nak to requester
			SendCacheNak(m, src);
		}
		else if (msg->Type()==mt_WritebackRequest)
		{
			const WritebackRequestMsg* m = (const WritebackRequestMsg*) msg;
			state = ds_BusySharedMemoryAccessWritebackRequest;

			secondRequest = m;
			secondRequestSrc = src;
		}
		else
		{
			PrintError("OnDirBusyShMemAcc", msg, "Unhandled message type");
		}
	}

   void OriginDirectory::OnDirectoryBusySharedMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirBusyShMemAccWbReq",*msg,"",src);
#endif
   	DirectoryState& state = directoryData.state;
   	const ReadMsg*& firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	const WritebackRequestMsg*& secondRequest = directoryData.secondRequest;
   	DebugAssertWithMessageID(secondRequest!=NULL, msg->MsgID());
   	NodeID& secondRequestSrc = directoryData.secondRequestSrc;
   	DebugAssertWithMessageID(secondRequestSrc!=InvalidNodeID, msg->MsgID());
   	NodeID& owner = directoryData.owner;

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;
			SendCacheNak(m, src);
		}
		else if (isFromMemory)
		{
			DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			PerformMemoryReadResponseCheck(m, src);
			DebugAssertWithMessageID(m->solicitingMessage==firstRequest->MsgID(), m->solicitingMessage);

			state = ds_Shared;

			// send shared reply to owner in directory
			ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, firstRequest);
			rrm->exclusiveOwnership = false;
			SendMessageToRemoteCache(rrm, owner);

			// send writeback exclusive ack to requester
			WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
			// soliciting message id should be the same as the WritebackMsg's id
			EM().InitializeEvictionResponseMsg(wam, secondRequest);
			wam->isExclusive = true;
			SendMessageToRemoteCache(wam, secondRequestSrc);

			// send memory write
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);

			// clear temporaries
			//EM().DisposeMsg(secondRequest);
			ClearTempDirectoryData(directoryData);
		} // else if (isFromMemory)
		else
		{
			PrintError("OnDirBusyShMemAccWbReq", msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnDirectoryDirectoryNak(const DirectoryNakMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirDirNak",*m,"",src);
#endif
		directoryNaksReceived++;
		DirectoryData& directoryData = GetDirectoryData(m->addr);
		OnDirectory(m, src, directoryData, false);
	}

   void OriginDirectory::OnDirectoryExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirEx",*msg,"",src);
#endif
   	const ReadMsg*& firstRequest = directoryData.firstRequest;
		DebugAssertWithMessageID(firstRequest==NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DebugAssertWithMessageID(firstRequestSrc==InvalidNodeID, msg->MsgID());
   	NodeID& owner = directoryData.owner;
   	NodeID& previousOwner = directoryData.previousOwner;
   	HashSet<NodeID>& sharers = directoryData.sharers;
   	DirectoryState& state = directoryData.state;

   	DebugAssertWithMessageID(sharers.size()==0, msg->MsgID());

   	if (msg->Type()==mt_Read)
   	{
   		const ReadMsg* m = (const ReadMsg*)msg;
   		if (owner==src)
   		{
   			state = ds_ExclusiveMemoryAccess;
				firstRequest = m;
				firstRequestSrc = src;
   			SendRequestToMemory(m);
   		}
   		else if (m->requestingExclusive && owner!=src)
   		{
   			state = ds_BusyExclusiveMemoryAccess;
   			SendRequestToMemory(m);
   			previousOwner = owner;
   			owner = src;
   			firstRequest = m;
   			firstRequestSrc = src;
   		}
   		else if (!m->requestingExclusive && owner!=src)
   		{ // shared read and owner!=requester
   			state = ds_BusySharedMemoryAccess;
   			SendRequestToMemory(m);
   			previousOwner = owner;
   			sharers.insert(owner);
   			owner = src;
   			firstRequest = m;
   			firstRequestSrc = src;
   		}
   	}
   	else if (msg->Type()==mt_WritebackRequest)
   	{
   		const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;
   		DebugAssertWithMessageID(owner==src, m->MsgID());

   		state = ds_Unowned;

			owner = InvalidNodeID;

   		// send writeback exclusive ack to requester
   		WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeEvictionResponseMsg(wam,m);
   		wam->isExclusive = true;
   		SendMessageToRemoteCache(wam, src);

   		// write to memory
   		WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeWriteMsg(wm, m);
   		SendRequestToMemory(wm);
   	}
   	else
   	{
			PrintError("OnDirEx", msg, "Unhandled message type");
   	}
	}

	void OriginDirectory::OnDirectoryExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirExMemAcc",*msg,"",src);
#endif
		DirectoryState& state = directoryData.state;
		const ReadMsg*& firstRequest = directoryData.firstRequest;
		DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
		NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*) msg;

			// send cachenak to requester
			SendCacheNak(m, src);
		}
		else if (isFromMemory)
		{
			DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;

			state = ds_Exclusive;

			// send exclusive reply to requester
			ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, firstRequest);
			rrm->blockAttached = true;
			rrm->exclusiveOwnership = true;
			rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, firstRequestSrc);

			// clear temp
			ClearTempDirectoryData(directoryData);
		} // else if (isFromMemory)
		else
		{
			PrintError("OnDirExMemAcc", msg, "Unhandled message type");
		}

		//EM().DisposeMsg(msg);
	}

	void OriginDirectory::OnDirectoryRead(const ReadMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirRead",*m,"",src);
#endif
		directoryReadsReceived++;
		DirectoryData& directoryData = GetDirectoryData(m->addr);
		OnDirectory(m, src, directoryData, false);
	}

	void OriginDirectory::OnDirectoryShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirSh",*msg,"",src);
#endif
		const ReadMsg*& firstRequest = directoryData.firstRequest;
		DebugAssertWithMessageID(firstRequest==NULL, msg->MsgID());
		NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DebugAssertWithMessageID(firstRequestSrc==InvalidNodeID, msg->MsgID());
		NodeID& owner = directoryData.owner;
		DebugAssertWithMessageID(owner!=InvalidNodeID, msg->MsgID());
		vector<LookupData <ReadMsg> >& pendingSharedReads = directoryData.pendingSharedReads;
		DebugAssertWithMessageID(pendingSharedReads.size()==0, msg->MsgID());
		HashSet<NodeID>& sharers = directoryData.sharers;
		DebugAssertWithMessageID(sharers.size()!=0, msg->MsgID());
		DirectoryState& state = directoryData.state;
		//int& pendingInvalidates = directoryData.pendingInvalidates;

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;
			if (!m->requestingExclusive)
			{
				state = ds_SharedMemoryAccess;
				sharers.insert(src);
				LookupData <ReadMsg> ld;
				ld.msg = m;
				ld.sourceNode = src;
				pendingSharedReads.push_back(ld);
				SendRequestToMemory(m);
			}
			else if (m->requestingExclusive && owner!=src && sharers.find(src)==sharers.end())
			{// if m is exclusive read and requester is not owner or in sharers
				state = ds_SharedExclusiveMemoryAccess;
				firstRequest = m;
				firstRequestSrc = src;
				SendRequestToMemory(m);
			}
			else if (m->requestingExclusive && (owner==src || sharers.find(src)!=sharers.end()))
			{// if requester is owner or is in sharers
				state = ds_Exclusive;

				// send exclusive reply (no data) with invalidates pending to requester
				ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(),m->GeneratingPC());
				EM().InitializeReadResponseMsg(rrm, m);
				rrm->blockAttached = false;
				rrm->exclusiveOwnership = true;
				rrm->pendingInvalidates = sharers.size();
				rrm->satisfied = true;
				SendMessageToRemoteCache(rrm, src);

				DebugAssertWithMessageID(owner!=InvalidNodeID, m->MsgID());
				// send invalidates to owner if it is not the requester
				if (owner!=src)
				{
					InvalidateMsg* im = EM().CreateInvalidateMsg(GetDeviceID(),m->GeneratingPC());
					im->addr = m->addr;
					im->newOwner = src;
					im->size = m->size;
					//im->solicitingMessage = m->MsgID();
					SendMessageToRemoteCache(im, owner);
				}

				// send invalidates to sharers that are not the requester
				for(HashSet<NodeID>::iterator i = sharers.begin(); i != sharers.end(); i++)
				{
					DebugAssertWithMessageID(*i!=InvalidNodeID, m->MsgID());
					if (*i!=src)
					{
						InvalidateMsg* im = EM().CreateInvalidateMsg(GetDeviceID(),m->GeneratingPC());
						im->addr = m->addr;
						im->newOwner = src;
						im->size = m->size;
						//im->solicitingMessage = m->MsgID();
						SendMessageToRemoteCache(im, *i);
					}
				}
				owner = src;
				sharers.clear();
			}//else if (m->requestingExclusive && (owner==src || sharers.find(src)!=sharers.end()))
			else
			{
				PrintError("OnDirSh",msg,"should be impossible to get here");
			}
		} // if (msg->Type()==mt_Read)
		else
		{
			PrintError("OnDirSh",msg, "Unhandled message type");
		}
	} // OriginDirectory::OnDirectoryShared

   void OriginDirectory::OnDirectorySharedExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirShExMemAcc",*msg,"",src);
#endif
   	DirectoryState& state = directoryData.state;
   	const ReadMsg*& firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	HashSet<NodeID>& sharers = directoryData.sharers;
   	NodeID& owner = directoryData.owner;

		if (isFromMemory)
		{
			DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			PerformMemoryReadResponseCheck(m, src);

			state = ds_Exclusive;

			// send invalidations to sharers and owner since we know that
				// the requester is not the owner or in sharers
			SendInvalidateMsg(firstRequest, owner, firstRequestSrc);
			for (HashSet<NodeID>::iterator i = sharers.begin(); i!=sharers.end(); i++)
			{
				SendInvalidateMsg(firstRequest, *i, firstRequestSrc);
			}

			// send exclusive reply with invalidates pending to requester
			ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, firstRequest);
			rrm->exclusiveOwnership = true;
			rrm->pendingInvalidates = sharers.size() + 1;
			rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, firstRequestSrc);

			sharers.clear();
			owner = firstRequestSrc;
			ClearTempDirectoryData(directoryData);
		}
		else if (msg->Type()==mt_Read)
		{
			// send nak to requester
			const ReadMsg* m = (const ReadMsg*) msg;
			SendCacheNak(m, src);
		}
		else
		{
			PrintError("OnShExMemAcc", msg, "Unhandled message type");
		}
	} // OnDirectorySharedExclusiveMemoryAccess

   void OriginDirectory::OnDirectorySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirShMemAcc",*msg,"",src);
#endif
		DirectoryState& state = directoryData.state;
		vector<LookupData<ReadMsg> >& pendingSharedReads = directoryData.pendingSharedReads;
		DebugAssertWithMessageID(pendingSharedReads.size()!=0, msg->MsgID());
		HashSet<NodeID>& sharers = directoryData.sharers;
		DebugAssertWithMessageID(sharers.size()!=0, msg->MsgID());

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;

			if (!m->requestingExclusive)
			{
				sharers.insert(src);
				LookupData<ReadMsg> ld;
				ld.msg = m;
				ld.sourceNode = src;
				pendingSharedReads.push_back(ld);
			}
			else
			{
				// send nak to requester
				SendCacheNak(m, src);
			}
		} // if (msg->Type()==mt_Read)
		else if (isFromMemory)
		{
			DebugAssertWithMessageID(msg->Type()==mt_ReadResponse, msg->MsgID());
			const ReadResponseMsg* m = (const ReadResponseMsg*)msg;
			PerformMemoryReadResponseCheck(m, src);

			state = ds_Shared;

			// for all messages in pending requests, send shared reply
			vector<LookupData<ReadMsg> >::iterator i;
			for (i=pendingSharedReads.begin(); i!=pendingSharedReads.end(); i++)
			{
				LookupData<ReadMsg> ld = *i;
				const ReadMsg*& read = ld.msg;
				NodeID& sourceNode = ld.sourceNode;

				ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeReadResponseMsg(rrm, read);
				rrm->blockAttached = true;
				rrm->exclusiveOwnership = false;
				rrm->satisfied = true;
				SendMessageToRemoteCache(rrm, sourceNode);

				//EM().DisposeMsg(read);
			}

			pendingSharedReads.clear();
		} // else if (isFromMemory)
		else
		{
			PrintError("OnDirShMemAcc", msg, "Unhandled message type");
		}
	} // OnDirectorySharedMemoryAccess

	void OriginDirectory::OnDirectoryUnowned(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirUnowned",*msg,"",src);
#endif
		DirectoryState& state = directoryData.state;
		const ReadMsg*& firstRequest = directoryData.firstRequest;
		DebugAssertWithMessageID(firstRequest==NULL, msg->MsgID());
		NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DebugAssertWithMessageID(firstRequestSrc==InvalidNodeID, msg->MsgID());
		NodeID& owner = directoryData.owner;

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;
			state = ds_ExclusiveMemoryAccess;
			firstRequest = m;
			firstRequestSrc = src;
			SendRequestToMemory(m);
			DebugAssertWithMessageID(owner==InvalidNodeID,m->MsgID());
			owner = src;
		}
		else
		{
			PrintError("OnDirUnowned",msg, "Unhandled message type");
		}
	}

	void OriginDirectory::OnDirectoryWriteback(const WritebackMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirWb",*m,"",src);
#endif
		directoryWritebacksReceived++;
		DirectoryData& directoryData = GetDirectoryData(m->addr);
		OnDirectory(m, src, directoryData, false);
	}

	void OriginDirectory::OnDirectoryWritebackRequest(const WritebackRequestMsg* m, NodeID src)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
		PrintDebugInfo("DirWbReq",*m,"",src);
#endif
		directoryWritebackRequestsReceived++;
		DirectoryData& directoryData = GetDirectoryData(m->addr);
		OnDirectory(m, src, directoryData, false);
	}

	/**
	 * Handles all the messages coming into the directory
	 * The message can come from the cache, memory, or the network
	 */
	void OriginDirectory::RecvMsg(const BaseMsg* msg, int connectionID)
	{
	   messagesReceived++;
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_COUNTERS
	   cout << threeStageDirectoryEraseDirectoryShareCounter << endl;
#endif
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_MSG_COUNT
	   cout << "OriginDirectory::RecvMsg: " << memoryDirectoryGlobalInt++ << ' ' << endl;
#endif
		DebugAssert(msg);

		if(connectionID == localCacheConnectionID)
		{
			RecvMsgCache(msg, InvalidNodeID);
		}
		else if (connectionID == localMemoryConnectionID)
		{
			//PerformMemoryResponseCheck(msg);
			RecvMsgDirectory(msg, InvalidNodeID, true);
		}
		else if(connectionID == remoteConnectionID)
		{
			DebugAssert(msg->Type() == mt_Network);
			const NetworkMsg* m = (const NetworkMsg*)msg;
			const BaseMsg* payload = m->payloadMsg;
			NodeID src = m->sourceNode;
			DebugAssert(m->destinationNode == nodeID);
			DebugAssert(src != InvalidNodeID);
			//EM().DisposeMsg(m);
			/*
			if (payload->Type()==mt_Read
				|| payload->Type()==mt_WritebackRequest
				|| payload->Type()==mt_Transfer
				|| payload->Type()==mt_Writeback
				|| payload->Type()==mt_DirectoryNak)
			{
				RecvMsgDirectory(payload, src, false);
			}
			else if (payload->Type()==mt_CacheNak
				|| payload->Type()==mt_SpeculativeReply
				|| payload->Type()==mt_ReadResponse
				|| payload->Type()==mt_ReadReply
				|| payload->Type()==mt_Intervention
				|| payload->Type()==mt_Invalidate
				|| payload->Type()==mt_InvalidateAck
				|| payload->Type()==mt_WritebackAck)
			{
				RecvMsgCache(payload, src);
			}
			*/

			if (payload->isCache)
			{
				RecvMsgCache(payload, src);
			}
			else if (payload->isDirectory)
			{
				RecvMsgDirectory(payload, src, false);
			}
			else
			{
				PrintError("RecvMsg", msg, "Unknown type received");
			}
		}
		else
		{
			DebugFail("Connection not a valid ID");
		}
	}

   void OriginDirectory::PrintDirectoryData(Address myAddress, MessageID myMessageID)
   {
      DebugAssert(directoryDataMap.find(myAddress)!=directoryDataMap.end());

      directoryDataMap[myAddress].print(myAddress, myMessageID);
   }

   void OriginDirectory::PrintError(const char* fromMethod, const BaseMsg *m)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	string output = ss.str();
   	cerr << output << endl;
		DebugFail("");
   }

   void OriginDirectory::PrintError(const char* fromMethod, const BaseMsg *m, const char* comment)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	ss << ": " << comment;
   	string output = ss.str();
   	cerr << output << endl;
		DebugFail("");
   }

	void OriginDirectory::PrintError(const char* fromMethod, const BaseMsg *m, int state)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	ss << " " << state;
   	string output = ss.str();
		cerr << output.c_str() << endl;
   	DebugFail("");
   }

	void OriginDirectory::PutErrorStringStream(stringstream& ss, const char* fromMethod, const BaseMsg*m)
	{
		ss << "OriginDir::"
			<< fromMethod
			<< ": msg received with id "
			<< m->MsgID()
			<< " type "
			<< m->Type()
			;
	}

   void OriginDirectory::PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation)
   {
      printBaseMemDeviceDebugInfo("3", fromMethod, myMessage, operation);
   }

   void OriginDirectory::PrintDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation, NodeID src)
   {
      printBaseMemDeviceDebugInfo("3", fromMethod, myMessage, operation, src);
   }

   void OriginDirectory::PrintDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(15) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(3) << GetDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " " << fromMethod
            << " " << operation << "(" << BaseMsg::convertNodeIDToDeviceID(id) << ")"
            << endl;
   }

   void OriginDirectory::PrintEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(15) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(2) << GetDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " " << fromMethod
            << " nodeID=" << id
            << " " << operation
            << endl;
   }

	void OriginDirectory::ProcessInterventionWhileInvalid(const InterventionMsg* m, NodeID src)
	{
		NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
		DebugAssertWithMessageID(src==dirNode, m->MsgID());
		DebugAssertWithMessageID(m->newOwner!=InvalidNodeID, m->solicitingMessage);
		/*
		const ReadMsg*& pendingSharedRead = cacheData.pendingSharedRead;
		DebugAssertWithMessageID(pendingSharedRead==NULL, m->MsgID());
		const ReadMsg*& pendingExclusiveRead = cacheData.pendingExclusiveRead;
		DebugAssertWithMessageID(pendingExclusiveRead==NULL, m->MsgID());
		*/

		if (m->requestingExclusive)
		{
			// send exclusive ack to requester
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(),m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, m);
			rrm->blockAttached = false;
			rrm->exclusiveOwnership = true;
			rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, m->newOwner);

			// send dirty transfer to directory
			TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(),m->GeneratingPC());
			EM().InitializeEvictionMsg(tm,m);
			tm->isDirty = true;
			SendMessageToDirectory(tm, false);
		}
		else
		{ // not requesting exclusive
			// send shared ack to requester
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, m);
			rrm->blockAttached = false;
			rrm->exclusiveOwnership = false;
			rrm->satisfied = true;
			SendMessageToRemoteCache(rrm, m->newOwner);

			// send shared transfer to directory
			TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionMsg(tm, m);
			tm->isShared = true;
			SendMessageToDirectory(tm, false);
		} // if (m->requestingExclusive)
	}

	void OriginDirectory::ProcessInvalidateWhileInvalid(const InvalidateMsg* m, NodeID src)
	{
		NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);
		DebugAssertWithMessageID(src==dirNode, m->MsgID());

		// send invalidate ack to requester
		InvalidateAckMsg* iam = EM().CreateInvalidateAckMsg(GetDeviceID(), m->GeneratingPC());
		EM().InitializeInvalidateResponseMsg(iam, m);
		iam->blockAttached = false;
		SendMessageToRemoteCache(iam, m->newOwner);	
	}

	/**
	performs certain error checking when encountering readResponse types of messages,
	which include ReadReplyMsg
	*/
	void OriginDirectory::ProcessReadResponse(const ReadResponseMsg* m, CacheData& cacheData)
	{
		vector<const ReadMsg*>& cacheDataPendingLocalReads = cacheData.cacheDataPendingLocalReads;

		DebugAssertWithMessageID(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end(), m->solicitingMessage);
		pendingLocalReads.erase(m->solicitingMessage);
		// there must be a message in cacheDataPendingLocalReads, so there shouldn't be an error using front()
		DebugAssertWithMessageID(cacheDataPendingLocalReads.size()>0, m->solicitingMessage);
		const ReadMsg* ref = cacheDataPendingLocalReads.front();
		DebugAssertWithMessageID(ref->MsgID()==m->solicitingMessage, m->solicitingMessage);
		EM().DisposeMsg(ref);
		// erase the first entry
		cacheDataPendingLocalReads.erase(cacheDataPendingLocalReads.begin());

		ProcessRemainingPendingLocalReads(cacheData);
	}

	void OriginDirectory::ProcessRemainingPendingLocalReads(CacheData& cacheData)
	{
		vector<const ReadMsg*>& cacheDataPendingLocalReads = cacheData.cacheDataPendingLocalReads;

		// process more pendingLocalReads if there are any
		if (cacheDataPendingLocalReads.size() > 0)
		{
			const ReadMsg* pendingRead = cacheDataPendingLocalReads.front();
			OnCache(pendingRead, InvalidNodeID, cacheData);
		}
	}
}
