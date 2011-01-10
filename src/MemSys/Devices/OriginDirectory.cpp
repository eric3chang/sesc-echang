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

// used in AutoDetermineDestSendMsg's member function calling
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

using std::cerr;
using std::cout;
using std::endl;

namespace Memory
{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
   int threeStageDirectoryEraseDirectoryShareCounter = 0;
#endif

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

	void OriginDirectory::dump(HashMap<Memory::MessageID, const Memory::BaseMsg*> &m)
	{
		DumpMsgTemplate<Memory::MessageID>(m);
	}
	void OriginDirectory::dump(HashMap<Address,LookupData<BaseMsg> > &m)
	{
		DumpLookupDataTemplate<Address>(m);
	}

	void OriginDirectory::AddDirectoryShare(Address a, NodeID id, bool exclusive)
	{
      DebugAssert(directoryNodeCalc->CalcNodeID(a)==nodeID);
		DirectoryData& b = directoryDataMap[a];
		DebugAssert(!exclusive || (b.sharers.size() == 0 && (b.owner == id || b.owner == InvalidNodeID)));
		if(b.owner == id || b.owner == InvalidNodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("AddDirectoryShare",a, id, "b.owner=");
#endif
         // unnecessary because owner is allowed to change when there are sharers
         //DebugAssert(b.sharers.find(id)==b.sharers.end());
			b.owner = id;
		}
		else if(b.sharers.find(id) == b.sharers.end())
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("AddDirectoryShare",a, id,
            ("exclusive="+to_string<bool>(exclusive)+" b.sharers.insert").c_str());
#endif
			b.sharers.insert(id);
		}
	}

	void OriginDirectory::ChangeOwnerToShare(Address a, NodeID id)
	{
      DebugAssert(directoryNodeCalc->CalcNodeID(a)==nodeID);
		DirectoryData& b = directoryDataMap[a];
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("ChangeOwnerToShare",a, id);
#endif
		DebugAssert(b.owner==id);
      DebugAssert(b.sharers.find(nodeID)==b.sharers.end());
      b.owner = InvalidNodeID;
      b.sharers.insert(id);
	}

	void OriginDirectory::EraseDirectoryShare(Address a, NodeID id)
	{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
	   threeStageDirectoryEraseDirectoryShareCounter++;
#endif
		DebugAssert(directoryDataMap.find(a) != directoryDataMap.end());
		DirectoryData& b = directoryDataMap[a];
		if(b.owner == id)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintEraseOwner("EraseDirectoryShare",a, id,"b.owner=InvalidNodeID");
#endif
			b.owner = InvalidNodeID;
			DebugAssert(b.sharers.find(id) == b.sharers.end());
		}
		else if(b.sharers.find(id) != b.sharers.end())
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
		   PrintDebugInfo("EraseDirectoryShare",a, id,"b.sharers.erase");
#endif
			b.sharers.erase(id);
		}
	}
	OriginDirectory::CacheData& OriginDirectory::GetCacheData(const BaseMsg* m)
	{
		Address currentAddress = BaseMemDevice::GetAddress(m);
		return cacheDataMap[currentAddress];
	}
	OriginDirectory::DirectoryData& OriginDirectory::GetDirectoryData(const BaseMsg* m)
	{
		Address currentAddress = BaseMemDevice::GetAddress(m);
		return directoryDataMap[currentAddress];
	}
	void OriginDirectory::HandleReceivedAllInvalidates(Address myAddress)
   {
      DebugAssert(waitingForInvalidates.find(myAddress)!=waitingForInvalidates.end());
      InvalidateData &id = waitingForInvalidates[myAddress];
      DebugAssert(id.msg!=NULL);
      ReadResponseMsg* rrm = (ReadResponseMsg*)id.msg;
      // the block should always be attached, according to the protocol,
         // that way, the read request can be guaranteed to be satisfied
      rrm->blockAttached = true;
      SendLocalReadResponse(rrm);
      std::vector<LookupData<ReadMsg> > pendingReads = id.pendingReads;
      // do not delete read response because it was sent to local
      if (pendingReads.size() != 0)
      {
      	/*
         for (std::vector<LookupData<ReadMsg> >::iterator i = pendingReads.begin();
            i < pendingReads.end(); i++)
         {// execute pending remote reads now that we sent the read response msg
            // we need satisfyTime+localSendTime for the readResponse to get processed
				CBOnRemoteRead::FunctionType* f = cbOnRemoteRead.Create();
            f->Initialize(this,i->msg,i->sourceNode);
				EM().ScheduleEvent(f,satisfyTime + localSendTime);
         }
         */
      }
      pendingReads.clear();
      waitingForInvalidates.erase(myAddress);
      // send invalidation complete message back to directory
   }

   // performs a directory fetch
	void OriginDirectory::PerformDirectoryFetch(const ReadMsg* msgIn, NodeID src)
	{
	   ReadMsg* m = (ReadMsg*)EM().ReplicateMsg(msgIn);
	   //EM().DisposeMsg(msgIn);
		m->onCompletedCallback = NULL;
		m->alreadyHasBlock = false;
      m->originalRequestingNode = src;
      m->addr = msgIn->addr;
      m->onCompletedCallback = NULL;
      m->requestingExclusive = msgIn->requestingExclusive;
      m->size = msgIn->size;
		DirectoryData& b = directoryDataMap[m->addr];
		NodeID target;
		bool isTargetMemory = false;
		// directoryLookup is true only for memory because we want to return a
		   // OnDirectoryBlockResponse from memory
		if(b.owner != InvalidNodeID)
		{
		   m->directoryLookup = false;
			target = b.owner;
		}
		else if(b.sharers.begin() != b.sharers.end())
		{
		   m->directoryLookup = false;
			target = *(b.sharers.begin());
		}
		/*
		else
		{
		   isTargetMemory = true;
		   m->directoryLookup = true;
			target = memoryNodeCalc->CalcNodeID(m->addr);
		}
		*/
		if(target == nodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               PrintDebugInfo("RemRead",*m,"PerDirFet(ReadMsg,src)",nodeID);
#endif
         //TODO 2010/09/02 Eric, change this because of 3-stage directory
         //OnRemoteRead should know to use m->requestingNode
			//OnRemoteRead(m, nodeID);//ERROR
         //OnRemoteRead(m, src);//ERROR
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->destinationNode = target;
			if (isTargetMemory)
			{
			   // change the source due to 3-stage
            nm->sourceNode = src;
            if (src != nodeID)
            {
               nm->isOverrideSource = true;
            }
			}
			else
			{
			   nm->sourceNode = nodeID;
			}
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, lookupTime + remoteSendTime);
		}
	}

   bool OriginDirectory::IsInPendingDirectoryNormalSharedRead(const ReadMsg *m)
   {
      DebugAssertWithMessageID(pendingDirectoryNormalSharedReads.find(m->addr)!=pendingDirectoryNormalSharedReads.end(),m->MsgID())
      AddrLookupIteratorPair iteratorPair = pendingDirectoryNormalSharedReads.equal_range(m->addr);
      for (; iteratorPair.first!=iteratorPair.second; ++iteratorPair.first)
      {
         LookupData<ReadMsg> &ld = iteratorPair.first->second;
         const ReadMsg *ref = ld.msg;
         if (ref->MsgID()==m->MsgID())
         {
            return true;
         }
      }
      return false;
   }

   // performs a directory fetch
	void OriginDirectory::PerformDirectoryFetch(const ReadMsg* msgIn,NodeID src,bool isExclusive,NodeID target)
	{
      DebugAssert(src != InvalidNodeID)
      DebugAssert(target != InvalidNodeID)
	   ReadMsg* m = (ReadMsg*)EM().ReplicateMsg(msgIn);
		m->alreadyHasBlock = false;
      m->originalRequestingNode = src;
      m->requestingExclusive = isExclusive;
      //TODO 2010/09/24 Eric
      // changed the block to return to directory first before doing anything else
      /*
		// directoryLookup is true only for memory because we want to return a
		   // OnDirectoryBlockResponse from memory
		if(target==memoryNode)
		{
		   m->directoryLookup = true;
		}
		else
      */
		{
		   m->directoryLookup = false;
		}
		if(target == nodeID)
		{
         DebugAssertWithMessageID(m->directoryLookup==false,m->MsgID())
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               PrintDebugInfo("RemRead",*m,"PerDirFet(m,src,isEx,targ)",nodeID);
#endif
			//OnRemoteRead(m, nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
			nm->destinationNode = target;
         /*
         //TODO 2010/09/24 Eric
         // changed it to not do 3-stage for memory
			if (target==memoryNode)
			{
			   // change the source due to 3-stage
            nm->sourceNode = src;
            if (src != nodeID)
            {
               nm->isOverrideSource = true;
            }
			}
			else
         */
			{
			   nm->sourceNode = nodeID;
			}
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, lookupTime + remoteSendTime);
		}
      // do not dispose message here, because the original OnLocalRead might still need it
      //EM().DisposeMsg(msgIn);
	}

   /**
   send directory request
   */
   void OriginDirectory::SendDirectoryNetworkMessage(const BaseMsg *msg, NodeID dest)
   {
   	if (dest==nodeID)
   	{
      	/*
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("RemEvic",*m,"LocEvic",dest);
#endif
         CBOnRemoteEviction::FunctionType* f = cbOnRemoteEviction.Create();
         f->Initialize(this,m,nodeID);
         // stagger the events to avoid some race conditions
         EM().ScheduleEvent(f, localSendTime);
         */
   		// wait for localSendTime
   		// call RecvMsg(m, remoteConnectionID)

   		//CBRecvMsg::FunctionType* f;
   	}
   	else
   	{
			NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), msg->GeneratingPC());
			DebugAssertWithMessageID(nm,msg->MsgID());
			nm->sourceNode = nodeID;
			nm->destinationNode = dest;
			nm->payloadMsg = msg;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
   	}
   }

   void OriginDirectory::SendDirectoryRequest(const ReadMsg *msgIn, bool isFromMemory)
   {
		NodeID dirNode = directoryNodeCalc->CalcNodeID(msgIn->addr);
		const ReadMsg* forward = (const ReadMsg*)EM().ReplicateMsg(msgIn);
		DirectoryData& directoryData = GetDirectoryData(msgIn);

		if(dirNode == nodeID)
		{
			CBOnDirectory::FunctionType* f = cbOnDirectory.Create();
			f->Initialize(this, forward, nodeID, &directoryData, isFromMemory);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			SendNetworkMessage(forward, dirNode);
		}
   }

   /**
   send local read response to the cache above
   */
   void OriginDirectory::SendLocalReadResponse(const ReadResponseMsg* m)
   {
      if (!m->evictionMessage)
      {// if m is not coming from eviction
         //ReadResponseMsg* r = (ReadResponseMsg*)EM().ReplicateMsg(m);
         DebugAssertWithMessageID(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end(),m->MsgID())
         const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];
         DebugAssertWithMessageID(m->solicitingMessage==ref->MsgID(),m->MsgID())
		   EM().DisposeMsg(ref);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		   PrintDebugInfo("DirectoryBlockResponse", *m,
		      ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
#endif
		   pendingLocalReads.erase(m->solicitingMessage);
         EraseReversePendingLocalRead(m,ref);
		   //EM().DisposeMsg(m);
		   //SendMsg(localConnectionID, m, satisfyTime + localSendTime);
      }
      else
      {
         // don't erase anything if the message is an evictionMessage
         //SendMsg(localConnectionID, m, satisfyTime + localSendTime);
      }
   }

   /**
   send a memory access complete message
   */
   void OriginDirectory::SendMemoryRequest(const BaseMsg *msg)
   {
   	if (msg->Type()==mt_Read
   			|| msg->Type()==mt_Write
   			|| msg->Type()==mt_Eviction)
   	{
   		BaseMsg *m = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,m,0);
   	}
   	else
   	{
   		PrintError("SendMemoryRequest", msg, "can't send current message type to memory");
   	}
   }

   void OriginDirectory::SendNetworkMessage(const BaseMsg *msg, NodeID dest)
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

   void OriginDirectory::SendRemoteEviction(const EvictionMsg *m,NodeID dest,const char *fromMethod)
   {
      if(dest == nodeID)
      {
      	/*
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("RemEvic",*m,"LocEvic",dest);
#endif
         CBOnRemoteEviction::FunctionType* f = cbOnRemoteEviction.Create();
         f->Initialize(this,m,nodeID);
         // stagger the events to avoid some race conditions
         EM().ScheduleEvent(f, localSendTime);
         */
      }
      else
      {
         NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
         nm->destinationNode = dest;
         nm->sourceNode = nodeID;
         nm->payloadMsg = m;
         SendMsg(remoteConnectionID, nm, remoteSendTime);
      }
   }
   void OriginDirectory::SendRemoteRead(const ReadMsg *m,NodeID dest,const char *fromMethod)
   {
      if (dest==nodeID)
      {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
         PrintDebugInfo("RemRead",*m,fromMethod,nodeID);
#endif
         //CALL_MEMBER_FN(*this,func) (msg,dest);
         /*
			CBOnRemoteRead::FunctionType* f = cbOnRemoteRead.Create();
         f->Initialize(this,m,nodeID);
			EM().ScheduleEvent(f,satisfyTime+localSendTime);
         return;
         */
      }
      else
      {
         NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(),m->GeneratingPC());
         nm->payloadMsg = m;
         nm->sourceNode = nodeID;
         nm->destinationNode = dest;
         SendMsg(remoteConnectionID, nm, remoteSendTime);
      }
   }
   	/**
	 * erase Node id as a share for Address a. If a is owned by id, check that there
	 * are no other shares
	 */
   void OriginDirectory::AddReversePendingLocalRead(const ReadMsg *m)
   {
      ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];
      HashMap<MessageID,const ReadMsg*> &exclusiveRead = myData.exclusiveRead;
      HashMap<MessageID,const ReadMsg*> &sharedRead = myData.sharedRead;
      HashMap<MessageID,const ReadMsg*> &myMap = m->requestingExclusive ? exclusiveRead : sharedRead;
      DebugAssertWithMessageID(myMap.find(m->MsgID())==myMap.end(),m->MsgID())
      myMap[m->MsgID()] = (ReadMsg*)EM().ReplicateMsg(m);
   }
   
   void OriginDirectory::ErasePendingDirectoryNormalSharedRead(const ReadResponseMsg *m)
   {/*
      BlockData &b = directoryDataMap[m->addr];
      AddrLookupIteratorPair iteratorPair = pendingDirectoryNormalSharedReads.equal_range(m->addr);
      bool hasFoundReadMsg = false;
      for (; iteratorPair.first != iteratorPair.second; ++iteratorPair.first)
      {
         LookupData<ReadMsg> &ld = iteratorPair.first->second;
         const ReadMsg *myReadMsg = ld.msg;
         if (myReadMsg->MsgID()==m->solicitingMessage)
         {
            hasFoundReadMsg = true;
            // because we replicated the read message that was actually sent
               // to the owner or sharer, the original message doesn't
               // have isNonBusySharedRead == true
            //DebugAssertWithMessageID(myReadMsg->isNonBusySharedRead,m->MsgID())
            EM().DisposeMsg(myReadMsg);
            break;
         }
      }
      DebugAssertWithMessageID(hasFoundReadMsg,m->MsgID())
      // delete from pendingDirectoryNormalSharedReads
      pendingDirectoryNormalSharedReads.erase(iteratorPair.first);
      return;*/
   }

   void OriginDirectory::AddPendingDirectoryNormalSharedRead(const ReadMsg *m, NodeID src)
   {/*
      LookupData <ReadMsg> ld;
      ld.msg = m;
      ld.sourceNode = src;
      AddrLookupPair insertedPair(m->addr,ld);
      pendingDirectoryNormalSharedReads.insert(insertedPair);
      return;*/
   }

   void OriginDirectory::ErasePendingLocalRead(const ReadResponseMsg *m)
   {
      DebugAssertWithMessageID(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end(),m->MsgID())
      const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];
      EM().DisposeMsg(ref);
      pendingLocalReads.erase(m->solicitingMessage);
   }

   void OriginDirectory::EraseReversePendingLocalRead(const ReadResponseMsg *m,const ReadMsg *ref)
   {
      DebugAssertWithMessageID(reversePendingLocalReads.find(m->addr)!=reversePendingLocalReads.end(),m->MsgID())
      ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];
      HashMap<MessageID,const ReadMsg*> &exclusiveRead = myData.exclusiveRead;
      HashMap<MessageID,const ReadMsg*> &sharedRead = myData.sharedRead;
      HashMap<MessageID,const ReadMsg*> &myMap = ref->requestingExclusive ? myData.exclusiveRead : myData.sharedRead;
      bool &isSatisfiedByEviction = ref->requestingExclusive ?
         myData.isExclusiveReadSatisfiedByEviction : myData.isSharedReadSatisfiedByEviction;
      DebugAssertWithMessageID(myMap.find(m->solicitingMessage)!=myMap.end(),m->MsgID())
      EM().DisposeMsg(myMap[m->solicitingMessage]);
      myMap.erase(m->solicitingMessage);
      isSatisfiedByEviction = false;

      if (exclusiveRead.size()==0 && sharedRead.size()==0)
      {
         reversePendingLocalReads.erase(m->addr);
      }
      return;
   }

   /**
    * Automatically determines whether to send to local or remote node
    */
   void OriginDirectory::AutoDetermineDestSendMsg(const BaseMsg* msg, NodeID dest, TimeDelta sendTime,
      OriginDirectoryMemFn func, const char* fromMethod, const char* toMethod)
   {
      if (dest==nodeID)
      {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
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
	   out << "messagesReceived:" << messagesReceived << std::endl;
	}

	void OriginDirectory::OnCacheRead(const ReadMsg* m, CacheData* cacheData)
   {
   	ReadRequestState& state = cacheData->readRequestState;
   	const ReadMsg* pendingExclusiveRead = cacheData->pendingExclusiveRead;
   	const ReadMsg* pendingSharedRead = cacheData->pendingSharedRead;

   	switch(state)
   	{
   	case rrs_NoPendingReads:
   		if (!m->requestingExclusive)
   		{
   			state = rrs_PendingSharedRead;
   			OnCache(m, InvalidNodeID, *cacheData);
   			DebugAssertWithMessageID(pendingSharedRead==NULL, m->MsgID());
   			pendingSharedRead = m;
   		}
   		else
   		{ // m is not requesting exclusive
   			state = rrs_PendingExclusiveRead;
   			OnCache(m, InvalidNodeID, *cacheData);
   			DebugAssertWithMessageID(pendingExclusiveRead==NULL, m->MsgID());
   			pendingExclusiveRead = m;
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

void OriginDirectory::OnDirectoryReadResponse(const ReadResponseMsg* m, NodeID src, CacheData* cacheData)
   {
   	ReadRequestState& state = cacheData->readRequestState;
   	const ReadMsg* pendingExclusiveRead = cacheData->pendingExclusiveRead;
   	const ReadMsg* pendingSharedRead = cacheData->pendingSharedRead;

   	switch(state)
   	{
   	case rrs_NoPendingReads:
   		// should not receive a read response to cache
   		PrintError("OnDirReadRes", m, state);
   		break;
   	case rrs_PendingExclusiveRead:
   		if (!m->satisfied)
   		{
   			state = rrs_NoPendingReads;
   			OnCache(m, InvalidNodeID, *cacheData);
   		}
   		else
   		{
   			// retry request
   			OnCache(m, InvalidNodeID, *cacheData);
   		}
   		break;
   	case rrs_PendingSharedRead:
   		// could be exclusive owned because it could be CEX
   		// DebugAssert(!m.isExclusiveOwned)
   		if (m->satisfied)
   		{
   			state = rrs_NoPendingReads;
   			OnCache(m, InvalidNodeID, *cacheData);
   			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage, m->MsgID());
   			pendingSharedRead = NULL;
   		}
   		else
   		{
   			// retry request
   			OnCache(m, InvalidNodeID, *cacheData);
   		}
   		break;
   	case rrs_PendingSharedReadExclusiveRead:
   		if (!m->satisfied)
   		{
   			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage, m->MsgID());
   			OnCache(m, InvalidNodeID, *cacheData);
   		}
   		else if (m->satisfied && m->exclusiveOwnership)
   		{
   			state = rrs_NoPendingReads;
   			DebugAssertWithMessageID(pendingExclusiveRead != NULL, m->MsgID());
   			DebugAssertWithMessageID(pendingSharedRead != NULL, m->MsgID());
   			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage
   					|| pendingExclusiveRead->MsgID()==m->solicitingMessage, m->MsgID());
   			pendingExclusiveRead = NULL;
   			pendingSharedRead = NULL;
   		}
   		else if (m->satisfied && !m->exclusiveOwnership)
   		{
				state = rrs_PendingExclusiveRead;
				// send the exclusive read request that was queued up
				SendDirectoryRequest(pendingExclusiveRead,false);
				OnCache(m, InvalidNodeID, *cacheData);
   		}
   		break;
   	default:
   		PrintError("OnDirReadRes", m, state);
   		break;
   	}
   }

void OriginDirectory::OnCache(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		CacheState& state = cacheData.cacheState;

		switch(state)
		{
		case cs_Exclusive:
			OnCacheExclusive(msg,src,cacheData);
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
	}

	void OriginDirectory::OnCacheExclusive(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheExclusiveWaitingForSpeculativeReply(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}
	
	void OriginDirectory::OnCacheInvalid(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		CacheState& state = cacheData.cacheState;

		if (msg->Type()==mt_Intervention)
		{
			const InterventionMsg* m = (const InterventionMsg*)msg;
			NodeID dirNode = directoryNodeCalc->CalcNodeID(m->addr);

			if (m->requestingExclusive)
			{
				// send exclusive ack to requester
				ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(),m->GeneratingPC());
				EM().InitializeReadResponseMsg(rrm,m);
				rrm->blockAttached = false;
				rrm->exclusiveOwnership = true;
				rrm->satisfied = true;
				SendNetworkMessage(rrm, src);

				// send dirty transfer to directory
				TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(),m->GeneratingPC());
				EM().InitializeEvictionMsg(tm,m);
				tm->isDirty = true;
				SendNetworkMessage(tm, dirNode);
			}
			else
			{ // not requesting exclusive
				// send shared ack to requester
				ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeReadResponseMsg(rrm, m);
				rrm->blockAttached = false;
				rrm->exclusiveOwnership = false;
				rrm->satisfied = true;
				SendNetworkMessage(rrm, src);

				// send shared transfer to directory
				TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeEvictionMsg(tm, m);
				tm->isShared = true;
				SendNetworkMessage(tm, dirNode);
			} // if (m->requestingExclusive)
		}// if (msgType==Intervention)
		else if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;

			if (!m->requestingExclusive)
			{
				// must be from local
				DebugAssert(src==InvalidNodeID);

				state = cs_WaitingForSharedReadResponse;

				// forward m to directory
				SendDirectoryRequest(m,false);
			}
			else
			{ // is requesting exclusive
				DebugAssert(src==InvalidNodeID);

				state = cs_WaitingForExclusiveReadResponse;

				// forward m to directory
				SendDirectoryRequest(m,false);
			} // if (!m->requestingExclusive)
		} // else if (msg->Type()==mt_Read)
		else
		{
			PrintError("OnCacheInvalid",msg);
		}
	}

	void OriginDirectory::OnCacheShared(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheSharedWaitingForSpeculativeReply(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheWaitingForExclusiveReadResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheWaitingForExclusiveResponseAck(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheWaitingForIntervention(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheWaitingForKInvalidatesJInvalidatesReceived(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheWaitingForSharedReadResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

		void OriginDirectory::OnCacheWaitingForWritebackBusyAck(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnCacheWaitingForWritebackResponse(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

		/**
	 * Received a readResponse from the directory for the cache
	 */
	void OriginDirectory::OnCacheWaitingForSharedResponseAck(const BaseMsg* msg, NodeID src, CacheData& cacheData)
	{
		;
	}

	void OriginDirectory::OnDirectory(const BaseMsg* msg, NodeID src, DirectoryData* directoryData, bool isFromMemory)
	{
		DirectoryState& state = directoryData->state;

		switch(state)
		{
		case ds_BusyExclusiveMemoryAccess:
			OnDirectoryBusyExclusiveMemoryAccess(msg, src, *directoryData, isFromMemory);
			break;
		case ds_BusyExclusive:
			OnDirectoryBusyExclusive(msg, src, *directoryData, isFromMemory);
			break;
		case ds_BusyExclusiveMemoryAccessWritebackRequest:
			OnDirectoryBusyExclusiveMemoryAccessWritebackRequest(msg, src, *directoryData, isFromMemory);
			break;
		case ds_BusyShared:
			OnDirectoryBusyShared(msg, src, *directoryData, isFromMemory);
			break;
		case ds_BusySharedMemoryAccess:
			OnDirectoryBusySharedMemoryAccess(msg, src, *directoryData, isFromMemory);
			break;
		case ds_BusySharedMemoryAccessWritebackRequest:
			OnDirectoryBusySharedMemoryAccessWritebackRequest(msg, src, *directoryData, isFromMemory);
			break;
		case ds_ExclusiveMemoryAccess:
			OnDirectoryExclusiveMemoryAccess(msg, src, *directoryData, isFromMemory);
			break;
		case ds_Exclusive:
			OnDirectoryExclusive(msg, src, *directoryData, isFromMemory);
			break;
		case ds_Shared:
			OnDirectoryShared(msg, src, *directoryData, isFromMemory);
			break;
		case ds_SharedExclusiveMemoryAccess:
			OnDirectorySharedExclusiveMemoryAccess(msg, src, *directoryData, isFromMemory);
			break;
		case ds_SharedMemoryAccess:
			OnDirectorySharedMemoryAccess(msg, src, *directoryData, isFromMemory);
			break;
		case ds_Unowned:
			OnDirectoryUnowned(msg, src, *directoryData, isFromMemory);
			break;
		default:
			PrintError("OnDir",msg, "Unimplemented directory state");
			break;
		}
	}

void OriginDirectory::OnDirectoryBusyExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryBusyExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryBusyExclusiveMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryBusyShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryBusySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryBusySharedMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
   	DirectoryState& state = directoryData.state;
   	HashSet<NodeID>& sharers = directoryData.sharers;
   	NodeID& owner = directoryData.owner;
   	NodeID& previousOwner = directoryData.previousOwner;

   	DebugAssertWithMessageID(sharers.size()==0, msg->MsgID());

   	if (msg->Type()==mt_Read)
   	{
   		const ReadMsg* m = (const ReadMsg*)msg;
   		if (owner==src)
   		{
   			state = ds_ExclusiveMemoryAccess;
   			SendMemoryRequest(m);
   		}
   		else if (m->requestingExclusive && owner!=src)
   		{
   			state = ds_BusyExclusiveMemoryAccess;
   			SendMemoryRequest(m);
   			previousOwner = owner;
   			owner = src;
   		}
   		else if (!m->requestingExclusive && owner!=src)
   		{ // shared read and owner!=requester
   			state = ds_BusySharedMemoryAccess;
   			SendMemoryRequest(m);
   			previousOwner = owner;
   			sharers.insert(owner);
   			owner = src;
   		}
   	}
   	else if (msg->Type()==mt_WritebackRequest)
   	{
   		const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;
   		DebugAssertWithMessageID(owner==src, m->MsgID());
   		state = ds_Unowned;

   		// send writeback exclusive ack to requester
   		WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeEvictionResponseMsg(wam,m);
   		wam->isExclusive = true;
   		//TODO
   	}
   	else
   	{
			PrintError("OnDirEx", msg);
   	}
	}

	void OriginDirectory::OnDirectoryExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

	void OriginDirectory::OnDirectoryShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		HashSet<NodeID>& sharers = directoryData.sharers;
		NodeID& owner = directoryData.owner;
		const BaseMsg* firstRequest = directoryData.firstRequest;
		DirectoryState& state = directoryData.state;
		int& pendingInvalidates = directoryData.pendingInvalidates;

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;
			if (!m->requestingExclusive)
			{
				state = ds_SharedMemoryAccess;
				sharers.insert(src);
				firstRequest = m;
				SendMemoryRequest(m);
			}
			else if (m->requestingExclusive && owner!=src && sharers.find(src)==sharers.end())
			{// if m is exclusive read and requester is not owner or in sharers
				state = ds_SharedExclusiveMemoryAccess;
				SendMemoryRequest(m);
			}
			else if (m->requestingExclusive && (owner==src || sharers.find(src)!=sharers.end()))
			{// if requester is owner or is in sharers
				state = ds_Exclusive;
				pendingInvalidates = sharers.size();

				// send exclusive reply (no data) with invalidates pending to requester
				ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(),m->GeneratingPC());
				EM().InitializeReadResponseMsg(rrm, m);
				rrm->blockAttached = false;
				rrm->exclusiveOwnership = true;
				rrm->pendingInvalidates = pendingInvalidates;
				rrm->satisfied = false;
				NetworkMsg* nm = EM().CreateNetworkMsg(GetDeviceID(), m->GeneratingPC());
				nm->sourceNode = nodeID;
				nm->destinationNode = src;
				nm->payloadMsg = rrm;
				SendMsg(remoteConnectionID, nm, remoteSendTime);

				DebugAssertWithMessageID(owner!=InvalidNodeID, m->MsgID());
				// send invalidates to owner if it is not the requester
				if (owner!=src)
				{
					InvalidateMsg* im = EM().CreateInvalidateMsg(GetDeviceID(),m->GeneratingPC());
					im->addr = m->addr;
					im->newOwner = src;
					im->size = m->size;
					im->solicitingMessage = m->MsgID();
					SendNetworkMessage(im, src);
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
						im->solicitingMessage = m->MsgID();
						SendNetworkMessage(im, *i);
					}
				}
				owner = src;
				sharers.clear();
			}//else if (m->requestingExclusive && (owner==src || sharers.find(src)!=sharers.end()))
		}//if (msg->Type()==mt_Read)
		else
		{
			PrintError("OnDirSh",msg);
		}// else msgType != read
	} // OriginDirectory::OnDirectoryShared

   void OriginDirectory::OnDirectorySharedExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectorySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

	void OriginDirectory::OnDirectoryUnowned(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		DirectoryState& state = directoryData.state;
		const BaseMsg* firstRequest = directoryData.firstRequest;
		NodeID& owner = directoryData.owner;

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;
			state = ds_ExclusiveMemoryAccess;
			DebugAssertWithMessageID(firstRequest==NULL, m->MsgID());
			firstRequest = m;
			SendMemoryRequest(m);
			DebugAssertWithMessageID(owner==InvalidNodeID,m->MsgID());
			owner = src;
		}
		else
		{
			PrintError("OnDirUnowned",msg);
		}
	}
	/**
	 * Handles all the messages coming into the directory
	 * The message can come from the cache, memory, or the network
	 */
	void OriginDirectory::RecvMsg(const BaseMsg* msg, int connectionID)
	{
	   messagesReceived++;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
	   cout << threeStageDirectoryEraseDirectoryShareCounter << endl;
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_MSG_COUNT
	   cout << "OriginDirectory::RecvMsg: " << memoryDirectoryGlobalInt++ << ' ' << endl;
#endif
		DebugAssert(msg);

		CacheData& cacheData = GetCacheData(msg);
		DirectoryData& directoryData = GetDirectoryData(msg);

		if(connectionID == localCacheConnectionID)
		{
			if (msg->Type()==mt_Read)
			{
				OnCacheRead((const ReadMsg*)msg,&cacheData);
			}
			else
			{
				OnCache(msg,InvalidNodeID,cacheData);
			}
		}
		else if (connectionID == localMemoryConnectionID)
		{
			OnDirectory(msg, InvalidNodeID, &directoryData, true);
		}
		else if(connectionID == remoteConnectionID)
		{
			DebugAssert(msg->Type() == mt_Network);
			const NetworkMsg* m = (const NetworkMsg*)msg;
			const BaseMsg* payload = m->payloadMsg;
			NodeID src = m->sourceNode;
			DebugAssert(m->destinationNode == nodeID);
			EM().DisposeMsg(m);

			if (payload->Type()==mt_ReadResponse
				|| payload->Type()==mt_ReadReply)
			{ // do special processing for mt_ReadResponse first
				OnDirectoryReadResponse((const ReadResponseMsg*)msg, src, &cacheData);
			}
			else if (payload->Type()==mt_Read
				|| payload->Type()==mt_WritebackRequest
				|| payload->Type()==mt_Transfer
				|| payload->Type()==mt_Writeback
				|| payload->Type()==mt_DirectoryNak)
			{
				OnDirectory(payload, src, &directoryData, false);
			}
			else if (payload->Type()==mt_CacheNak
				|| payload->Type()==mt_SpeculativeReply
				|| payload->Type()==mt_Intervention
				|| payload->Type()==mt_Invalidate
				|| payload->Type()==mt_InvalidateAck
				|| payload->Type()==mt_WritebackAck)
			{
				OnCache(msg, src, cacheData);
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

      bool isSharedBusy = (pendingDirectoryBusySharedReads.find(myAddress)!=pendingDirectoryBusySharedReads.end());
      bool isExclusiveBusy = (pendingDirectoryBusyExclusiveReads.find(myAddress)!=pendingDirectoryBusyExclusiveReads.end());
      bool hasPendingMemAccess = (pendingMainMemAccesses.find(myAddress)!=pendingMainMemAccesses.end());
      bool isWaitingForReadResponse = (waitingForReadResponse.find(myAddress)!=waitingForReadResponse.end());
      
      directoryDataMap[myAddress].print(myAddress, myMessageID,isSharedBusy, isExclusiveBusy, hasPendingMemAccess
         ,isWaitingForReadResponse);
   }

   void OriginDirectory::PrintError(const char* fromMethod, const BaseMsg *m)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	string output = ss.str();
		DebugFail(output.c_str());
   }

   void OriginDirectory::PrintError(const char* fromMethod, const BaseMsg *m, const char* comment)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	ss << ": " << comment;
   	string output = ss.str();
		DebugFail(output.c_str());
   }

	void OriginDirectory::PrintError(const char* fromMethod, const BaseMsg *m, int state)
   {
   	stringstream ss;
   	PutErrorStringStream(ss, fromMethod, m);
   	ss << " " << state;
   	string output = ss.str();
   	DebugFail(output.c_str());
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

	/**
	 * readMsgArray in this method could be coupled with Eclipse's debugger to
	 * see what elements are in pendingDirectoryBusySharedReads. The reason for using
	 * a regular array instead of a vector is because Eclipse can view
	 * Array elements directly, but not elements in STL containers.
	 */
	void OriginDirectory::PrintPendingDirectoryBusySharedReads()
	{
	   HashMap<Address, LookupData<ReadMsg> >::const_iterator myIterator;


	   myIterator = pendingDirectoryBusySharedReads.begin();
      cout << "OriginDirectory::printPendingDirectorySharedReads: ";

      int size = pendingDirectoryBusySharedReads.size();
      cout << "size=" << size << endl;
      ReadMsg const** readMsgArray = new ReadMsg const*[size];
      //readMsgVector[0] = new ReadMsg const;

      int i = 0;
	   for (myIterator = pendingDirectoryBusySharedReads.begin();
	         myIterator != pendingDirectoryBusySharedReads.end(); myIterator++)
	   {
	      cout << "\t\tmyIterator->first=" << myIterator->first << " ";
	      readMsgArray[i] = myIterator->second.msg;
	      i++;
	   }
      delete [] readMsgArray;
      readMsgArray = NULL;
	}

   /**
    * readMsgArray in this method could be coupled with Eclipse's debugger to
    * see what elements are in pendingLocalReads. The reason for using
    * a regular array instead of a vector is because Eclipse can view
    * Array elements directly, but not elements in STL containers.
    */
	void OriginDirectory::PrintPendingLocalReads()
	{
      HashMap<MessageID, const ReadMsg*>::const_iterator myIterator;

      myIterator = pendingLocalReads.begin();

      cout << "OriginDirectory::printPendingLocalReads: ";

      int size = pendingLocalReads.size();
      cout << "size=" << size;

      ReadMsg const ** readMsgArray = new ReadMsg const *[size];

      int i = 0;
      for (myIterator = pendingLocalReads.begin();
            myIterator != pendingLocalReads.end(); ++myIterator)
      {
         cout << "myIterator->first=" << myIterator->first << " ";
         //const ReadMsg *myReadMsg = myIterator->second.msg;
         readMsgArray[i] = myIterator->second;
         i++;
      }
      
      delete [] readMsgArray;
      readMsgArray = NULL;
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
}
