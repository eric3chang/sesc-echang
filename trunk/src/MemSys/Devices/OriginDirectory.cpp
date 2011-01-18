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
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_COUNTERS
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
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("AddDirectoryShare",a, id, "b.owner=");
#endif
         // unnecessary because owner is allowed to change when there are sharers
         //DebugAssert(b.sharers.find(id)==b.sharers.end());
			b.owner = id;
		}
		else if(b.sharers.find(id) == b.sharers.end())
		{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
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
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintDebugInfo("ChangeOwnerToShare",a, id);
#endif
		DebugAssert(b.owner==id);
      DebugAssert(b.sharers.find(nodeID)==b.sharers.end());
      b.owner = InvalidNodeID;
      b.sharers.insert(id);
	}


   void OriginDirectory::ClearTempCacheData(CacheData& cacheData)
   {
   	EM().DisposeMsg(cacheData.firstReply);
   	cacheData.firstReply = NULL;
   	cacheData.firstReplySrc = InvalidNodeID;
   	cacheData.invalidAcksReceived = -1;
   }

   void OriginDirectory::ClearTempDirectoryData(DirectoryData& directoryData)
   {
   	EM().DisposeMsg(directoryData.firstRequest);
   	directoryData.firstRequest = NULL;
   	directoryData.firstRequestSrc = InvalidNodeID;
   	directoryData.previousOwner = InvalidNodeID;
   }

	void OriginDirectory::EraseDirectoryShare(Address a, NodeID id)
	{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_COUNTERS
	   threeStageDirectoryEraseDirectoryShareCounter++;
#endif
		DebugAssert(directoryDataMap.find(a) != directoryDataMap.end());
		DirectoryData& b = directoryDataMap[a];
		if(b.owner == id)
		{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
         PrintEraseOwner("EraseDirectoryShare",a, id,"b.owner=InvalidNodeID");
#endif
			b.owner = InvalidNodeID;
			DebugAssert(b.sharers.find(id) == b.sharers.end());
		}
		else if(b.sharers.find(id) != b.sharers.end())
		{
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_DIRECTORY_DATA
		   PrintDebugInfo("EraseDirectoryShare",a, id,"b.sharers.erase");
#endif
			b.sharers.erase(id);
		}
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
      //m->originalRequestingNode = src;
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
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
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
      //m->originalRequestingNode = src;
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
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
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

	void OriginDirectory::PerformMemoryReadResponseCheck(const ReadResponseMsg *m, NodeID src)
	{
		DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());
		DebugAssertWithMessageID(pendingMemoryReadAccesses.find(m->solicitingMessage)!=pendingMemoryReadAccesses.end(), m->MsgID());
		const ReadMsg* rm = pendingMemoryReadAccesses[m->MsgID()];
		EM().DisposeMsg(rm);
		pendingMemoryReadAccesses.erase(m->MsgID());
	}

	void OriginDirectory::PerformMemoryWriteResponseCheck(const WriteResponseMsg *m, NodeID src)
	{
		DebugAssertWithMessageID(src==InvalidNodeID, m->MsgID());
		DebugAssertWithMessageID(pendingMemoryWriteAccesses.find(m->solicitingMessage)!=pendingMemoryWriteAccesses.end(), m->MsgID());
		const WriteMsg* rm = pendingMemoryWriteAccesses[m->MsgID()];
		EM().DisposeMsg(rm);
		pendingMemoryWriteAccesses.erase(m->MsgID());
	}

	void OriginDirectory::RecvMsgCache(const BaseMsg *msg, NodeID src)
	{
		if (msg->Type()==mt_Read)
		{
			DebugAssertWithMessageID(src==InvalidNodeID, msg->MsgID());
			const ReadMsg* m = (const ReadMsg*) msg;
			OnCacheRead(m);
		}
		else if (msg->Type()==mt_ReadResponse || msg->Type()==mt_ReadReply)
		{
			const ReadResponseMsg* m = (const ReadResponseMsg*) msg;
			OnCacheReadResponse(m, src);
		}
		else
		{
			CacheData& cacheData = GetCacheData(msg);
			OnCache(msg, src, cacheData);
		}
	}

	void OriginDirectory::RecvMsgDirectory(const BaseMsg *msg, NodeID src, bool isFromMemory)
	{
		if (msg->Type()==mt_WriteResponse)
		{
			DebugAssertWithMessageID(isFromMemory, msg->MsgID());
			const WriteResponseMsg* m = (const WriteResponseMsg*)msg;
			PerformMemoryWriteResponseCheck(m, src);
		}
		else
		{
			OnDirectory(msg, src, isFromMemory);
		}
	}

   void OriginDirectory::SendCacheNak(const ReadMsg* m, NodeID dest)
   {
   	CacheNakMsg* cnk = EM().CreateCacheNakMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeBaseNakMsg(cnk, m);
   	SendMessageToCache(cnk, dest);
   } // SendCacheNak

   void OriginDirectory::SendDirectoryNak(const ReadMsg* m, NodeID dest)
   {
   	DirectoryNakMsg* dnk = EM().CreateDirectoryNakMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeBaseNakMsg(dnk, m);
   	SendMessageToDirectory(dnk, dest);
   } // SendDirectoryNak

   void OriginDirectory::SendInvalidateMsg(const ReadMsg* m, NodeID dest, NodeID newOwner)
   {
   	InvalidateMsg* im = EM().CreateInvalidateMsg(GetDeviceID(), m->GeneratingPC());
   	EM().InitializeInvalidateMsg(im, m);
   	im->newOwner = newOwner;
   	SendMessageToCache(im, dest);
   } // SendInvalidateMsg

	void OriginDirectory::SendMessageToCache(const BaseMsg *msg, NodeID dest)
	{
		DebugAssertWithMessageID(dest!=InvalidNodeID, msg->MsgID());
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

   void OriginDirectory::SendMessageToDirectory(const BaseMsg *msg, bool isFromMemory)
   {
   	Address addr = BaseMemDevice::GetAddress(msg);
		NodeID dirNode = directoryNodeCalc->CalcNodeID(addr);
		DebugAssertWithMessageID(dirNode!=InvalidNodeID, msg->MsgID());
		const BaseMsg* forward = EM().ReplicateMsg(msg);

		if(dirNode == nodeID)
		{
			// schedule OnDirectory to be called
			CBRecvMsgDirectory::FunctionType* f = cbRecvMsgDirectory.Create();
			f->Initialize(this, forward, nodeID, isFromMemory);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			SendMessageToNetwork(forward, dirNode);
		}
   }

   /**
   send local read response to the cache above
   */
   void OriginDirectory::SendLocalReadResponse(const ReadResponseMsg* m)
   {
      //if (!m->evictionMessage)
   	if (true)
      {// if m is not coming from eviction
         //ReadResponseMsg* r = (ReadResponseMsg*)EM().ReplicateMsg(m);
         DebugAssertWithMessageID(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end(),m->MsgID())
         const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];
         DebugAssertWithMessageID(m->solicitingMessage==ref->MsgID(),m->MsgID())
		   EM().DisposeMsg(ref);
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_PENDING_LOCAL_READS
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
   void OriginDirectory::SendRequestToMemory(const BaseMsg *msg)
   {
   	if (msg->Type()==mt_Read)
   	{
   		DebugAssertWithMessageID(pendingMemoryReadAccesses.find(msg->MsgID())==pendingMemoryReadAccesses.end(),msg->MsgID());
   		const ReadMsg* m = (const ReadMsg*) msg;
   		pendingMemoryReadAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,0);
   	}
   	else if (msg->Type()==mt_Write)
   	{
   		DebugAssertWithMessageID(pendingMemoryWriteAccesses.find(msg->MsgID())==pendingMemoryWriteAccesses.end(),msg->MsgID());
   		const WriteMsg* m = (const WriteMsg*) msg;
   		pendingMemoryWriteAccesses[msg->MsgID()] = m;
   		BaseMsg *forward = EM().ReplicateMsg(msg);
   		// using 0 for send time because the time is taken care of in TestMemory
   		SendMsg(localMemoryConnectionID,forward,0);
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

   void OriginDirectory::SendRemoteEviction(const EvictionMsg *m,NodeID dest,const char *fromMethod)
   {
      if(dest == nodeID)
      {
      	/*
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
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
#ifdef MEMORY_ORIGIN_DIRECTORY_DEBUG_VERBOSE
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

	void OriginDirectory::OnCacheRead(const ReadMsg* m)
   {
		CacheData& cacheData = GetCacheData(m->addr);
   	ReadRequestState& state = cacheData.readRequestState;
   	const ReadMsg* pendingExclusiveRead = cacheData.pendingExclusiveRead;
   	const ReadMsg* pendingSharedRead = cacheData.pendingSharedRead;

   	switch(state)
   	{
   	case rrs_NoPendingReads:
   		if (!m->requestingExclusive)
   		{
   			state = rrs_PendingSharedRead;
   			OnCache(m, InvalidNodeID, cacheData);
   			DebugAssertWithMessageID(pendingSharedRead==NULL, m->MsgID());
   			pendingSharedRead = m;
   		}
   		else
   		{ // m is not requesting exclusive
   			state = rrs_PendingExclusiveRead;
   			OnCache(m, InvalidNodeID, cacheData);
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

	void OriginDirectory::OnCacheReadResponse(const ReadResponseMsg* m, NodeID src)
   {
		CacheData& cacheData = GetCacheData(m->addr);
   	ReadRequestState& state = cacheData.readRequestState;
   	const ReadMsg* pendingExclusiveRead = cacheData.pendingExclusiveRead;
   	const ReadMsg* pendingSharedRead = cacheData.pendingSharedRead;

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
   			OnCache(m, InvalidNodeID, cacheData);
   		}
   		else
   		{
   			// retry request
   			OnCache(m, InvalidNodeID, cacheData);
   		}
   		break;
   	case rrs_PendingSharedRead:
   		// could be exclusive owned because it could be CEX
   		// DebugAssert(!m.isExclusiveOwned)
   		if (m->satisfied)
   		{
   			state = rrs_NoPendingReads;
   			OnCache(m, InvalidNodeID, cacheData);
   			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage, m->MsgID());
   			pendingSharedRead = NULL;
   		}
   		else
   		{
   			// retry request
   			OnCache(m, InvalidNodeID, cacheData);
   		}
   		break;
   	case rrs_PendingSharedReadExclusiveRead:
   		if (!m->satisfied)
   		{
   			DebugAssertWithMessageID(pendingSharedRead->MsgID()==m->solicitingMessage, m->MsgID());
   			OnCache(m, InvalidNodeID, cacheData);
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
				SendMessageToDirectory(pendingExclusiveRead,false);
				OnCache(m, InvalidNodeID, cacheData);
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

			if (m->requestingExclusive)
			{
				// send exclusive ack to requester
				ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(),m->GeneratingPC());
				EM().InitializeReadResponseMsg(rrm,m);
				rrm->blockAttached = false;
				rrm->exclusiveOwnership = true;
				rrm->satisfied = true;
				SendMessageToCache(rrm, src);

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
				SendMessageToCache(rrm, src);

				// send shared transfer to directory
				TransferMsg* tm = EM().CreateTransferMsg(GetDeviceID(), m->GeneratingPC());
				EM().InitializeEvictionMsg(tm, m);
				tm->isShared = true;
				SendMessageToDirectory(tm, false);
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
				SendMessageToDirectory(m,false);
			}
			else
			{ // is requesting exclusive
				DebugAssert(src==InvalidNodeID);

				state = cs_WaitingForExclusiveReadResponse;

				// forward m to directory
				SendMessageToDirectory(m,false);
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

	void OriginDirectory::OnDirectory(const BaseMsg* msg, NodeID src, bool isFromMemory)
	{
		DirectoryData& directoryData = GetDirectoryData(msg);
		DirectoryState& state = directoryData.state;

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
		;
	}

   void OriginDirectory::OnDirectoryBusyExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
   	DirectoryState& state = directoryData.state;
   	const WritebackRequestMsg* secondRequest = directoryData.secondRequest;
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


   		// send speculative reply to requester
   		//TODO
   	}
	}

   void OriginDirectory::OnDirectoryBusyExclusiveMemoryAccessWritebackRequest(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
		;
	}

   void OriginDirectory::OnDirectoryBusyShared(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
   	DirectoryState& state = directoryData.state;
   	NodeID& owner = directoryData.owner;
   	NodeID& previousOwner = directoryData.previousOwner;
   	HashSet<NodeID>& sharers =  directoryData.sharers;

		if (msg->Type()==mt_Writeback)
		{
			const WritebackMsg* m = (const WritebackMsg*) msg;
			state = ds_Shared;

			// write to memory
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);
		}
		else if (msg->Type()==mt_Transfer)
		{
			state = ds_Shared;
		}
		else if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*) msg;

			// send cache nak to requester
			CacheNakMsg* cnm = EM().CreateCacheNakMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeBaseNakMsg(cnm, m);
			SendMessageToCache(cnm, src);
		}
		else if (msg->Type()==mt_WritebackRequest)
		{
			const WritebackRequestMsg* m = (const WritebackRequestMsg*)msg;

			state = ds_Shared;

			// send shared response to owner
			DebugAssertWithMessageID(owner!=InvalidNodeID, m->MsgID());
			ReadResponseMsg* rrm = EM().CreateReadResponseMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, m);
			rrm->exclusiveOwnership = false;
			rrm->satisfied = true;
			SendMessageToCache(rrm, owner);

			// send writeback busy ack to requester
			WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(wam, m);
			wam->isBusy = true;
			SendMessageToCache(wam, src);

			// memory write
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);
		}//else if (msg->Type()==mt_WritebackRequest)
		else if (msg->Type()==mt_DirectoryNak)
		{
			const DirectoryNakMsg* m = (const DirectoryNakMsg*)msg;
			DebugAssertWithMessageID(src==previousOwner, m->MsgID());
			state = ds_Exclusive;
			owner = previousOwner;
			sharers.clear();
		} // else if (msg->Type()==mt_DirectoryNak)
		else
		{
			PrintError("OnDirBusySh", msg, "Unexpected message type");
		}

		// dispose message
		EM().DisposeMsg(msg);
	}

   void OriginDirectory::OnDirectoryBusySharedMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
   	const ReadMsg* firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
   	DirectoryState& state = directoryData.state;
   	NodeID& previousOwner = directoryData.previousOwner;
   	const WritebackRequestMsg* secondRequest = directoryData.secondRequest;
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
			im->requestingExclusive = false;
			SendMessageToCache(im, previousOwner);

			// send speculative reply to requester
			SpeculativeReplyMsg* srm = EM().CreateSpeculativeReplyMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(srm, firstRequest);
			srm->blockAttached = true;
			srm->satisfied = true;
			SendMessageToCache(srm, src);
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
   	DirectoryState& state = directoryData.state;
   	const ReadMsg* firstRequest = directoryData.firstRequest;
   	DebugAssertWithMessageID(firstRequest!=NULL, msg->MsgID());
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;
   	DebugAssertWithMessageID(firstRequestSrc!=InvalidNodeID, msg->MsgID());
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

			state = ds_Shared;

			// send shared reply to owner in directory
			ReadReplyMsg* rrm = EM().CreateReadReplyMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeReadResponseMsg(rrm, firstRequest);
			rrm->exclusiveOwnership = false;
			SendMessageToCache(rrm, owner);

			// send writeback exclusive ack to requester
			WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeEvictionResponseMsg(wam, m);
			wam->isExclusive = true;
			wam->solicitingMessage = firstRequest->MsgID();
			SendMessageToCache(wam, src);

			// send memory write
			WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
			EM().InitializeWriteMsg(wm, m);
			SendRequestToMemory(wm);

			// clear temporaries
			ClearTempDirectoryData(directoryData);
		} // else if (isFromMemory)
		else
		{
			PrintError("OnDirBusyShMemAccWbReq", msg, "Unhandled message type");
		}
	}

   void OriginDirectory::OnDirectoryExclusive(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
   	DirectoryState& state = directoryData.state;
   	HashSet<NodeID>& sharers = directoryData.sharers;
   	NodeID& owner = directoryData.owner;
   	NodeID& previousOwner = directoryData.previousOwner;
   	const ReadMsg* firstRequest = directoryData.firstRequest;
   	NodeID& firstRequestSrc = directoryData.firstRequestSrc;

   	DebugAssertWithMessageID(sharers.size()==0, msg->MsgID());

   	if (msg->Type()==mt_Read)
   	{
   		const ReadMsg* m = (const ReadMsg*)msg;
   		if (owner==src)
   		{
   			state = ds_ExclusiveMemoryAccess;
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

   		// send writeback exclusive ack to requester
   		WritebackAckMsg* wam = EM().CreateWritebackAckMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeEvictionResponseMsg(wam,m);
   		wam->isExclusive = true;
   		SendMessageToCache(wam, src);

   		// write to memory
   		WriteMsg* wm = EM().CreateWriteMsg(GetDeviceID(), m->GeneratingPC());
   		EM().InitializeWriteMsg(wm, m);
   		SendRequestToMemory(wm);
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
		NodeID& firstRequestSrc = directoryData.firstRequestSrc;
		DirectoryState& state = directoryData.state;
		//int& pendingInvalidates = directoryData.pendingInvalidates;

		if (msg->Type()==mt_Read)
		{
			const ReadMsg* m = (const ReadMsg*)msg;
			if (!m->requestingExclusive)
			{
				state = ds_SharedMemoryAccess;
				sharers.insert(src);
				firstRequest = m;
				SendRequestToMemory(m);
			}
			else if (m->requestingExclusive && owner!=src && sharers.find(src)==sharers.end())
			{// if m is exclusive read and requester is not owner or in sharers
				state = ds_SharedExclusiveMemoryAccess;
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
					SendMessageToCache(im, src);
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
						SendMessageToCache(im, *i);
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
			PrintError("OnDirSh",msg);
		}
	} // OriginDirectory::OnDirectoryShared

   void OriginDirectory::OnDirectorySharedExclusiveMemoryAccess(const BaseMsg* msg, NodeID src, DirectoryData& directoryData, bool isFromMemory)
	{
   	DirectoryState& state = directoryData.state;
   	const ReadMsg* firstRequest = directoryData.firstRequest;
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
			SendMessageToCache(rrm, firstRequestSrc);

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
		;
	} // OnDirectorySharedMemoryAccess

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
			SendRequestToMemory(m);
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
			EM().DisposeMsg(m);

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
				RecvMsgCache(msg, src);
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
