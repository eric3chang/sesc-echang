#include "ThreeStageDirectory.h"
#include "../Debug.h"
#include "../MSTypes.h"
#include "../Messages/AllMessageTypes.h"
#include "../Configuration.h"
#include "../EventManager.h"
#include "../Connection.h"
#include "to_string.h"

// used in AutoDetermineDestSendMsg's member function calling
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

// toggles debug messages
#define MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
#define MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_MSG_COUNT
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_EXCLUSIVE_READS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_EVICTION
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_READS

using std::cerr;
using std::cout;
using std::endl;

namespace Memory
{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
   int threeStageDirectoryEraseDirectoryShareCounter = 0;
#endif

	NodeID ThreeStageDirectory::HashedPageCalculator::CalcNodeID(Address addr) const
	{
		int index = ((int)(addr / pageSize) % elementCount);
		DebugAssert(index >= 0 && index < (int)nodeSet.size());
		return nodeSet[index];
	}

	void ThreeStageDirectory::HashedPageCalculator::Initialize(const RootConfigNode& node)
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

   void ThreeStageDirectory::HandleInterventionComplete(const BaseMsg *msgIn, bool isPendingExclusive)
   {
      if (msgIn->Type()==mt_ReadResponse)
      {
         ReadResponseMsg *m = (ReadResponseMsg*) msgIn;
         if (waitingForInterventionComplete.find(m->solicitingMessage)==waitingForInterventionComplete.end())
         {
            DebugAssert(waitingForInterventionComplete[m->solicitingMessage].rrm==NULL);
            waitingForInterventionComplete[m->solicitingMessage].rrm = m;
         }
         else
         {
            DebugAssert(waitingForInterventionComplete[m->solicitingMessage].icm != NULL);
            EM().DisposeMsg(waitingForInterventionComplete[m->solicitingMessage].icm);
            waitingForInterventionComplete.erase(m->solicitingMessage);
            if (isPendingExclusive)
            {
               DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end());
               EM().DisposeMsg(pendingDirectoryBusyExclusiveReads[m->addr].msg);
               pendingDirectoryBusyExclusiveReads.erase(m->addr);
            }
            else
            {
               DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end());
               EM().DisposeMsg(pendingDirectoryBusySharedReads[m->addr].msg);
               pendingDirectoryBusySharedReads.erase(m->addr);
            }
            DebugAssert(waitingForReadResponse.find(m->addr)==waitingForReadResponse.end());
            EM().DisposeMsg(m);            
         }
      }
      else if (msgIn->Type()==mt_InterventionComplete)
      {
         InterventionCompleteMsg *m = (InterventionCompleteMsg*) msgIn;
         if (waitingForInterventionComplete.find(m->solicitingMessage)==waitingForInterventionComplete.end())
         {
            DebugAssert(waitingForInterventionComplete[m->solicitingMessage].icm==NULL);
            waitingForInterventionComplete[m->solicitingMessage].icm = m;
         }
         else
         {
            DebugAssert(waitingForInterventionComplete[m->solicitingMessage].rrm != NULL);
            EM().DisposeMsg(waitingForInterventionComplete[m->solicitingMessage].rrm);
            waitingForInterventionComplete.erase(m->solicitingMessage);
            if (isPendingExclusive)
            {
               DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end());
               EM().DisposeMsg(pendingDirectoryBusyExclusiveReads[m->addr].msg);
               pendingDirectoryBusyExclusiveReads.erase(m->addr);
               DebugAssert(waitingForReadResponse.find(m->addr)==waitingForReadResponse.end());
            }
            else
            {
               DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end());
               EM().DisposeMsg(pendingDirectoryBusySharedReads[m->addr].msg);
               pendingDirectoryBusySharedReads.erase(m->addr);
               DebugAssert(waitingForReadResponse.find(m->addr)==waitingForReadResponse.end());
            }
            EM().DisposeMsg(m);
         }
      }
      else
      {
         DebugFail("invalid message type");
      }
   }

   void ThreeStageDirectory::HandleReceivedAllInvalidates(Address myAddress)
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
         for (std::vector<LookupData<ReadMsg> >::iterator i = pendingReads.begin();
            i < pendingReads.end(); i++)
         {// execute pending remote reads now that we sent the read response msg
            // we need satisfyTime+localSendTime for the readResponse to get processed
				CBOnRemoteRead::FunctionType* f = cbOnRemoteRead.Create();
            f->Initialize(this,i->msg,i->sourceNode);
				EM().ScheduleEvent(f,satisfyTime + localSendTime);
         }
      }
      pendingReads.clear();
      waitingForInvalidates.erase(myAddress);
      // send invalidation complete message back to directory
      NodeID directoryNode = directoryNodeCalc->CalcNodeID(myAddress);
      InvalidationCompleteMsg *icm = EM().CreateInvalidationCompleteMsg(getDeviceID());
      icm->addr = myAddress;
      icm->solicitingMessage = rrm->solicitingMessage;
      AutoDetermineDestSendMsg(icm,directoryNode,remoteSendTime,
         &ThreeStageDirectory::OnRemoteInvalidationComplete,"HanRecAllInv","OnRemInvCom");
   }

   // performs a directory fetch
	void ThreeStageDirectory::PerformDirectoryFetch(const ReadMsg* msgIn, NodeID src)
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
		BlockData& b = directoryData[m->addr];
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
		else
		{
		   isTargetMemory = true;
		   m->directoryLookup = true;
			target = memoryNodeCalc->CalcNodeID(m->addr);
		}
		if(target == nodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemRead",*m,"PerDirFet(ReadMsg,src)",nodeID);
#endif
         //TODO 2010/09/02 Eric, change this because of 3-stage directory
         //OnRemoteRead should know to use m->requestingNode
			OnRemoteRead(m, nodeID);//ERROR
         //OnRemoteRead(m, src);//ERROR
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
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

   bool ThreeStageDirectory::IsInPendingDirectoryNormalSharedRead(const ReadMsg *m)
   {
      DebugAssert(pendingDirectoryNormalSharedReads.find(m->addr)!=pendingDirectoryNormalSharedReads.end());
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
	void ThreeStageDirectory::PerformDirectoryFetch(const ReadMsg* msgIn,NodeID src,bool isExclusive,NodeID target)
	{
      DebugAssert(src != InvalidNodeID);
      DebugAssert(target != InvalidNodeID);
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
         DebugAssert(m->directoryLookup==false);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemRead",*m,"PerDirFet(m,src,isEx,targ)",nodeID);
#endif
			OnRemoteRead(m, nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
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
   send directory block request
   */
   void ThreeStageDirectory::SendDirectoryBlockRequest(const ReadMsg *forward)
   {
		NodeID remoteNode = directoryNodeCalc->CalcNodeID(forward->addr);

		if(remoteNode == nodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
		   printDebugInfo("OnDirBlkRequ",*forward,"SendDirBlkReq");
#endif
			CBOnDirectoryBlockRequest::FunctionType* f = cbOnDirectoryBlockRequest.Create();
			f->Initialize(this,forward,nodeID);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), forward->GeneratingPC());
			DebugAssert(nm);
			nm->destinationNode = remoteNode;
			nm->sourceNode = nodeID;
			nm->payloadMsg = forward;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
   }

   /**
   send local read response to the cache above
   */
   void ThreeStageDirectory::SendLocalReadResponse(const ReadResponseMsg* m)
   {
      if (!m->evictionMessage)
      {// if m is coming from eviction
         //ReadResponseMsg* r = (ReadResponseMsg*)EM().ReplicateMsg(m);
         DebugAssert(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end());
         const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];
         DebugAssert(m->solicitingMessage==ref->MsgID());
		   EM().DisposeMsg(ref);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		   printDebugInfo("OnDirectoryBlockResponse", *m,
		      ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
#endif
		   pendingLocalReads.erase(m->solicitingMessage);
         EraseReversePendingLocalRead(m,ref);
		   //EM().DisposeMsg(m);
		   SendMsg(localConnectionID, m, satisfyTime + localSendTime);
      }
      else
      {
         //DebugAssert(pendingLocalReads.find(m->solicitingMessage)==pendingLocalReads.end());
         DebugAssert(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end());
         const ReadMsg *ref = pendingLocalReads[m->solicitingMessage];
         DebugAssert(m->solicitingMessage==ref->MsgID());
         EM().DisposeMsg(ref);
         pendingLocalReads.erase(m->solicitingMessage);
         EraseReversePendingLocalRead(m,ref);
         SendMsg(localConnectionID, m, satisfyTime + localSendTime);
      }
   }

   /**
   send a memory access complete message
   */
   void ThreeStageDirectory::SendMemAccessComplete(Address addr, NodeID directoryNode)
   {
      MemAccessCompleteMsg* reply = EM().CreateMemAccessCompleteMsg(getDeviceID());
      reply->addr = addr;

      // send to src, which should be the directory
      AutoDetermineDestSendMsg(reply,directoryNode,remoteSendTime,
         &ThreeStageDirectory::OnRemoteMemAccessComplete,"OnDirBlkResp","OnRemoteMemAccCom");
   }

   void ThreeStageDirectory::SendRemoteRead(const ReadMsg *m,NodeID dest,const char *fromMethod)
   {
      if (dest==nodeID)
      {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
         printDebugInfo("OnRemRead",*m,fromMethod,nodeID);
#endif
         //CALL_MEMBER_FN(*this,func) (msg,dest);
			CBOnRemoteRead::FunctionType* f = cbOnRemoteRead.Create();
         f->Initialize(this,m,nodeID);
			EM().ScheduleEvent(f,satisfyTime+localSendTime);
         return;
      }
      else
      {
         NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
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
	void ThreeStageDirectory::EraseDirectoryShare(Address a, NodeID id)
	{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
	   threeStageDirectoryEraseDirectoryShareCounter++;
#endif
		DebugAssert(directoryData.find(a) != directoryData.end());
		BlockData& b = directoryData[a];
		if(b.owner == id)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printEraseOwner("EraseDirectoryShare",a, id,"b.owner=InvalidNodeID");
#endif
			b.owner = InvalidNodeID;
			DebugAssert(b.sharers.find(id) == b.sharers.end());
		}
		else if(b.sharers.find(id) != b.sharers.end())
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
		   printDebugInfo("EraseDirectoryShare",a, id,"b.sharers.erase");
#endif
			b.sharers.erase(id);
		}
	}

	void ThreeStageDirectory::AddDirectoryShare(Address a, NodeID id, bool exclusive)
	{
      DebugAssert(directoryNodeCalc->CalcNodeID(a)==nodeID);
		BlockData& b = directoryData[a];
		DebugAssert(!exclusive || (b.sharers.size() == 0 && (b.owner == id || b.owner == InvalidNodeID)));
		if(b.owner == id || b.owner == InvalidNodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("AddDirectoryShare",a, id, "b.owner=");
#endif
         // unnecessary because owner is allowed to change when there are sharers
         //DebugAssert(b.sharers.find(id)==b.sharers.end());
			b.owner = id;
		}
		else if(b.sharers.find(id) == b.sharers.end())
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("AddDirectoryShare",a, id,
            ("exclusive="+to_string<bool>(exclusive)+" b.sharers.insert").c_str());
#endif
			b.sharers.insert(id);
		}
	}

   void ThreeStageDirectory::AddReversePendingLocalRead(const ReadMsg *m)
   {
      ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];
      HashMap<MessageID,const ReadMsg*> &exclusiveRead = myData.exclusiveRead;
      HashMap<MessageID,const ReadMsg*> &sharedRead = myData.sharedRead;
      HashMap<MessageID,const ReadMsg*> &myMap = m->requestingExclusive ? exclusiveRead : sharedRead;
      DebugAssert(myMap.find(m->MsgID())==myMap.end());
      myMap[m->MsgID()] = (ReadMsg*)EM().ReplicateMsg(m);
   }
   
   void ThreeStageDirectory::ErasePendingDirectoryNormalSharedRead(const ReadResponseMsg *m)
   {/*
      BlockData &b = directoryData[m->addr];
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
            //DebugAssert(myReadMsg->isNonBusySharedRead);
            EM().DisposeMsg(myReadMsg);
            break;
         }
      }
      DebugAssert(hasFoundReadMsg);
      // delete from pendingDirectoryNormalSharedReads
      pendingDirectoryNormalSharedReads.erase(iteratorPair.first);
      return;*/
   }

   void ThreeStageDirectory::AddPendingDirectoryNormalSharedRead(const ReadMsg *m, NodeID src)
   {/*
      LookupData <ReadMsg> ld;
      ld.msg = m;
      ld.sourceNode = src;
      AddrLookupPair insertedPair(m->addr,ld);
      pendingDirectoryNormalSharedReads.insert(insertedPair);
      return;*/
   }

   void ThreeStageDirectory::EraseReversePendingLocalRead(const ReadResponseMsg *m,const ReadMsg *ref)
   {
      DebugAssert(reversePendingLocalReads.find(m->addr)!=reversePendingLocalReads.end());
      ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];
      HashMap<MessageID,const ReadMsg*> &exclusiveRead = myData.exclusiveRead;
      HashMap<MessageID,const ReadMsg*> &sharedRead = myData.sharedRead;
      HashMap<MessageID,const ReadMsg*> &myMap = ref->requestingExclusive ? myData.exclusiveRead : myData.sharedRead;
      bool &isSatisfiedByEviction = ref->requestingExclusive ?
         myData.isExclusiveReadSatisfiedByEviction : myData.isSharedReadSatisfiedByEviction;
      DebugAssert(myMap.find(m->solicitingMessage)!=myMap.end());
      EM().DisposeMsg(myMap[m->solicitingMessage]);
      myMap.erase(m->solicitingMessage);
      isSatisfiedByEviction = false;

      if (exclusiveRead.size()==0 && sharedRead.size()==0)
      {
         reversePendingLocalReads.erase(m->addr);
      }
      return;
   }

   void ThreeStageDirectory::ChangeOwnerToShare(Address a, NodeID id)
	{
      DebugAssert(directoryNodeCalc->CalcNodeID(a)==nodeID);
		BlockData& b = directoryData[a];
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("ChangeOwnerToShare",a, id);
#endif
		DebugAssert(b.owner==id);
      DebugAssert(b.sharers.find(nodeID)==b.sharers.end());
      b.owner = InvalidNodeID;
      b.sharers.insert(id);
	}

   /**
   Automatically determines whether to send to local or remote node
   */
   void ThreeStageDirectory::AutoDetermineDestSendMsg(const BaseMsg* msg, NodeID dest, TimeDelta sendTime,
      ThreeStageDirectoryMemFn func, const char* fromMethod, const char* toMethod)
   {
      if (dest==nodeID)
      {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
         printDebugInfo(toMethod,*msg,fromMethod,nodeID);
#endif
         CALL_MEMBER_FN(*this,func) (msg,dest);
         return;
      }
      else
      {
         NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),msg->GeneratingPC());
         nm->payloadMsg = msg;
         nm->sourceNode = nodeID;
         nm->destinationNode = dest;
         SendMsg(remoteConnectionID, nm, sendTime);
      }
   }

   void ThreeStageDirectory::writeToMainMemory(const EvictionMsg *m)
   {
      writeToMainMemory(m->addr, m->GeneratingPC(), m->size);
   }

   void ThreeStageDirectory::writeToMainMemory(const InvalidateResponseMsg *m)
   {
      writeToMainMemory(m->addr, m->GeneratingPC(), m->size);
   }

   void ThreeStageDirectory::writeToMainMemory(const ReadResponseMsg *m)
   {
      writeToMainMemory(m->addr, m->GeneratingPC(), m->size);
   }

   /**
   write block to main memory
   */
   void ThreeStageDirectory::writeToMainMemory(Address addr, Address generatingPC, size_t size)
   {
      DebugAssert(addr && size);
      WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), generatingPC);
      wm->addr = addr;
      wm->size = size;
      wm->onCompletedCallback = NULL;
      DebugAssert(pendingMemoryWrites.find(wm->MsgID())==pendingMemoryWrites.end());
      pendingMemoryWrites.insert(wm->MsgID());

      NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), generatingPC);
      nm->sourceNode = nodeID;
      nm->destinationNode = memoryNodeCalc->CalcNodeID(addr);
      nm->payloadMsg = wm;
      SendMsg(remoteConnectionID, nm, remoteSendTime);
   } // void ThreeStageDirectory::writeToMainMemory(const BaseMsg* msgIn)

	/**
	 * the message came from the local (cpu) side. It is a read msg
	 */
	void ThreeStageDirectory::OnLocalRead(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printDebugInfo("OnLocalRead", *m,
		      ("pendingLocalReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
      // if we have a request with permissions greater, dispose it
      ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];
      HashMap<MessageID,const ReadMsg*> &exclusiveRead = myData.exclusiveRead;
      HashMap<MessageID,const ReadMsg*> &sharedRead = myData.sharedRead;
      if (m->requestingExclusive)
      {
         if(exclusiveRead.size()!=0)
         {
            EM().DisposeMsg(m);
            return;
         }
      }
      else
      {// shared read
         if (exclusiveRead.size()!=0 || sharedRead.size()!=0)
         {
            EM().DisposeMsg(m);
            return;
         }
      }

		DebugAssert(pendingLocalReads.find(m->MsgID()) == pendingLocalReads.end());
		pendingLocalReads[m->MsgID()] = m;
      AddReversePendingLocalRead(m);
		ReadMsg* forward = (ReadMsg*)EM().ReplicateMsg(m);
		forward->onCompletedCallback = NULL;
		forward->directoryLookup = true;
		forward->originalRequestingNode = nodeID;

      // if this address is being evicted, send read when evictionResponse is received
      if (pendingEviction.find(m->addr)!=pendingEviction.end())
      {
         DebugAssert(waitingForEvictionResponse.find(m->addr)==waitingForEvictionResponse.end());
         waitingForEvictionResponse[m->addr] = forward;
         return;
      }

      SendDirectoryBlockRequest(forward);
	}
	void ThreeStageDirectory::OnLocalReadResponse(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_ReadResponse);
      ReadResponseMsg* m = (ReadResponseMsg*)msgIn;
		DebugAssert(pendingRemoteReads.find(m->solicitingMessage)!=pendingRemoteReads.end());
      LookupData<ReadMsg> &d = pendingRemoteReads[m->solicitingMessage];
      DebugAssert(d.msg->originalRequestingNode != InvalidNodeID);
      //TODO 2010/09/23 Eric
      // can be unsatisfied if the owner is still waiting for response from memory
         // or if the intervention message arrived before data arrived from memory
      //DebugAssert(m->satisfied);

		if (d.msg->isIntervention)
		{
         if (m->satisfied)
         {
            //TODO 2010/09/23 Eric
            // block doesn't have to be attached if owner is still waiting for response from memory
               // or if the intervention message arrived before data arrived from memory
            //DebugAssert(m->blockAttached);

            // send response back to the requester, have to make sure that this arrives before the directory message arrives
            ReadResponseMsg* forward = (ReadResponseMsg*)EM().ReplicateMsg(m);
            forward->originalRequestingNode = d.msg->originalRequestingNode;
            // changed this to true because of 3-stage directory
            forward->directoryLookup = true;
            forward->isIntervention = true;
            forward->isWaitingForInvalidateUnlock = d.msg->isWaitingForInvalidateUnlock;
            //forward->isSpeculative = d.msg->isSpeculative;
            // the destination node is the original requesting node, and not the directory
            AutoDetermineDestSendMsg(forward,d.msg->originalRequestingNode,remoteSendTime+lookupTime,
                  &ThreeStageDirectory::OnDirectoryBlockResponse,"OnLocalReadRes","OnDirBlkResp");

            // send response back to the directory, have to make sure that this arrives after the requester message arrives
            ReadResponseMsg* dirResponse = (ReadResponseMsg*)EM().ReplicateMsg(m);
            DebugAssert(d.msg->originalRequestingNode != InvalidNodeID);
            dirResponse->originalRequestingNode = d.msg->originalRequestingNode;
            dirResponse->directoryLookup = false;
            dirResponse->isIntervention = true;
            //forward->isSpeculative = d.msg->isSpeculative;
            AutoDetermineDestSendMsg(dirResponse,d.sourceNode,remoteSendTime+lookupTime,
                  &ThreeStageDirectory::OnRemoteReadResponse,"OnLocalReadRes","OnRemReadRes");
         } // if satisfied
         else  // unsatisfied, 2 possibilities:
         {//1. block was evicted before intervention message was received by this node
            // 2. this node was read before an OnRemoteInvalidateResponse brings the appropriate data.
            // the second case should never happen, because it was taken care of by locks on the directory side

            // wait for special eviction response to arrive, ignore intervention message
            /*
            DebugAssert(waitingForEvictionBusyAck.find(m->addr)==waitingForEvictionBusyAck.end());
            waitingForEvictionBusyAck[m->addr] = m;
            */

            // only needs to send response back to the directory it is case 1: evicted
            ReadResponseMsg* dirResponse = (ReadResponseMsg*)EM().ReplicateMsg(m);
            DebugAssert(d.msg->originalRequestingNode != InvalidNodeID);
            dirResponse->originalRequestingNode = d.msg->originalRequestingNode;
            dirResponse->directoryLookup = false;
            dirResponse->isIntervention = true;
            //forward->isSpeculative = d.msg->isSpeculative;
            AutoDetermineDestSendMsg(dirResponse,d.sourceNode,remoteSendTime+lookupTime,
                  &ThreeStageDirectory::OnRemoteReadResponse,"OnLocalReadRes","OnRemReadRes");
         }
		} // if (d.msg->isInterventionShared)
      // not doing speculative replies right now
      /*
      else if (d.msg->isSpeculative)
		{
         // send response back to the requester
         ReadResponseMsg* forward = (ReadResponseMsg*)EM().ReplicateMsg(m);
         // changed this to true because of 3-stage directory
         forward->directoryLookup = true;
         forward->isSpeculative = true;
         forward->originalRequestingNode = d.msg->originalRequestingNode;
         AutoDetermineDestSendMsg(forward,d.msg->originalRequestingNode,remoteSendTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnLocReadRes","OnDirBlkResp");
		}
      */
      else
      {
         // send response back to the requester
         ReadResponseMsg* blockResponse = (ReadResponseMsg*)EM().ReplicateMsg(m);
         blockResponse->originalRequestingNode = d.msg->originalRequestingNode;
         // changed this to true because of 3-stage directory
         blockResponse->directoryLookup = true;
         blockResponse->isIntervention = false;
         blockResponse->isWaitingForInvalidateUnlock = d.msg->isWaitingForInvalidateUnlock;
         blockResponse->originalRequestingNode = d.msg->originalRequestingNode;
         //blockResponse->isSpeculative = d.msg->isSpeculative;
         // the destination node is the original requesting node, and not the directory
         AutoDetermineDestSendMsg(blockResponse,d.msg->originalRequestingNode,remoteSendTime+lookupTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnLocReadRes","OnDirBlkResp");

         // if it is coming from a non-busy shared read, send response to directory to
            // notify directory to erase the message
         /*
         //TODO
         // 2010/10/19 Eric
         if (d.msg->isNonBusySharedRead)
         {
            NodeID directoryNode = directoryNodeCalc->CalcNodeID(m->addr);
            ReadResponseMsg* toDir = (ReadResponseMsg*)EM().ReplicateMsg(m);
            toDir->directoryLookup = false;
            toDir->originalRequestingNode = d.msg->originalRequestingNode;
            AutoDetermineDestSendMsg(toDir,directoryNode,remoteSendTime,
               &ThreeStageDirectory::OnRemoteReadResponse,"OnLocReadRes","OnRemReadRes");
         }
         */
      }
      // cannot dispose this message because it's in OnLocalRead
      //EM().DisposeMsg(pendingRemoteReads[m->solicitingMessage].msg);
      pendingRemoteReads.erase(m->solicitingMessage);
      EM().DisposeMsg(m);
      return;
	} // ThreeStageDirectory::OnLocalReadResponse()

	void ThreeStageDirectory::OnLocalWrite(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Write);
      WriteMsg* m = (WriteMsg*)msgIn;
		NodeID id = directoryNodeCalc->CalcNodeID(m->addr);
		WriteResponseMsg* wrm = EM().CreateWriteResponseMsg(getDeviceID(),m->GeneratingPC());
		wrm->addr = m->addr;
		wrm->size = m->size;
		wrm->solicitingMessage = m->MsgID();
		SendMsg(localConnectionID, wrm, localSendTime);
		if(id == nodeID)
		{
			OnRemoteWrite(m,nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
			DebugAssert(nm);
			nm->sourceNode = nodeID;
			nm->destinationNode = id;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void ThreeStageDirectory::OnLocalEviction(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Eviction);
      EvictionMsg* m = (EvictionMsg*)msgIn;
		DebugAssert(pendingEviction.find(m->addr) == pendingEviction.end());
      EvictionMsg* myEvictionMsg = (EvictionMsg*)EM().ReplicateMsg(m);
      pendingEviction[m->addr] = myEvictionMsg;
		EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(getDeviceID(),m->GeneratingPC());
		erm->addr = m->addr;
		erm->size = m->size;
		erm->solicitingMessage = m->MsgID();
		SendMsg(localConnectionID, erm, localSendTime);
      // send eviction message to directory
		NodeID directoryNode = directoryNodeCalc->CalcNodeID(m->addr);
		if(directoryNode == nodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemEvic",*m,"OnLocalEviction",nodeID);
#endif
         CBOnRemoteEviction::FunctionType* f = cbOnRemoteEviction.Create();
         f->Initialize(this,m,nodeID);
         // stagger the events so they don't all try to request at the same time
         EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
			nm->destinationNode = directoryNode;
			nm->sourceNode = nodeID;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void ThreeStageDirectory::OnLocalInvalidateResponse(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_InvalidateResponse);
      InvalidateResponseMsg* m = (InvalidateResponseMsg*)msgIn;
      DebugAssert(pendingRemoteInvalidates.find(m->solicitingMessage) != pendingRemoteInvalidates.end());
      LookupData<InvalidateMsg>& d = pendingRemoteInvalidates[m->solicitingMessage];
		DebugAssert(d.msg->MsgID() == m->solicitingMessage);

      //TODO 2010/09/13 Eric
      //if(nodeID != d.sourceNode)
      // send message to newOwner instead of sourceNode/directory
      if (nodeID != d.msg->newOwner)
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),d.msg->GeneratingPC());
			nm->sourceNode = nodeID;
         //TODO 2010/09/13 Eric
         //nm->destinationNode = d.sourceNode;
         nm->destinationNode = d.msg->newOwner;
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
		else
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteInvalidateResponse",*m,"OnLocalInvalidateResponse",nodeID);
#endif
			OnRemoteInvalidateResponse(m,nodeID);
		}
		EM().DisposeMsg(d.msg);
      // do not delete m here, it will be deleted in OnRemoteInvalidateResponse
      //EM().DisposeMsg(m);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
		printDebugInfo("OnLocalInvalidateResponse",*m,
		   ("PRI.erase("+to_string<Address>(d.msg->addr)+")").c_str());
#endif
      pendingRemoteInvalidates.erase(m->solicitingMessage);
	}

	void ThreeStageDirectory::OnRemoteRead(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
		DebugAssert(!m->directoryLookup);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_READS
		printDebugInfo("OnRemRead", *m,
		      ("pendingRemoteReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
      // if we are still waiting for invalidates, put it in a queue
      if (waitingForInvalidates.find(m->addr)!=waitingForInvalidates.end())
      {
         LookupData <ReadMsg> ld;
         ld.msg = m;
         ld.sourceNode = src;
         waitingForInvalidates[m->addr].pendingReads.push_back(ld);
         return;
      }

		DebugAssert(pendingRemoteReads.find(m->MsgID()) == pendingRemoteReads.end());
      DebugAssert(m->originalRequestingNode != InvalidNodeID);
		pendingRemoteReads[m->MsgID()].msg = m;
		pendingRemoteReads[m->MsgID()].sourceNode = src;
		SendMsg(localConnectionID, EM().ReplicateMsg(m), localSendTime);
	} // ThreeStageDirectory::OnRemoteRead()

	void ThreeStageDirectory::OnRemoteReadResponse(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_ReadResponse);
      ReadResponseMsg* m = (ReadResponseMsg*)msgIn;
		DebugAssert(!m->directoryLookup);
      bool isPendingDirectorySharedReads = (pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end());
      bool isPendingDirectoryExclusiveReads = (pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end());
      bool isPendingMainMemAccesses = (pendingMainMemAccesses.find(m->addr)!=pendingMainMemAccesses.end());
      bool isWaitingForInterventionComplete = (waitingForInterventionComplete.find(m->solicitingMessage)!=waitingForInterventionComplete.end());
      bool isWaitingForReadResponse = (waitingForReadResponse.find(m->addr)!=waitingForReadResponse.end());
      bool isWaitingForSharedRead = (pendingDirectoryNormalSharedReads.find(m->addr)!=pendingDirectoryNormalSharedReads.end());

      // is not used to indicate busy
      bool isWaitingForInvalidationComplete = waitingForInvalidationComplete.find(m->addr)!=waitingForInvalidationComplete.end();

		DebugAssert(isPendingDirectorySharedReads || isPendingDirectoryExclusiveReads || isPendingMainMemAccesses || isWaitingForReadResponse
         || isWaitingForSharedRead);
      // m->addr should only exist in one of the HashMaps
      int pendingCount = isPendingDirectorySharedReads + isPendingDirectoryExclusiveReads + isPendingMainMemAccesses + isWaitingForReadResponse
         + isWaitingForSharedRead;
		DebugAssert(pendingCount == 1);
		DebugAssert(directoryData.find(m->addr) != directoryData.end());

      if (src == memoryNodeCalc->CalcNodeID(m->addr))
      {
         // if it came from memory
         //DebugAssert(pendingIgnoreInterventions.find(m->addr)==pendingIgnoreInterventions.end());
         DebugAssert(pendingMainMemAccesses.find(m->addr)!=pendingMainMemAccesses.end());

         // send message from directory back to the original requester
         ReadResponseMsg* newMsg = (ReadResponseMsg*)EM().ReplicateMsg(m);
         newMsg->directoryLookup = true;
         std::vector<LookupData<ReadMsg> > pendingMainMemAccessesVector = pendingMainMemAccesses[m->addr];
         std::vector<LookupData<ReadMsg> >::iterator i = pendingMainMemAccessesVector.begin();

         DebugAssert(pendingMainMemAccessesVector.size()>0);

         // if there is more than one message pending
         if (pendingMainMemAccessesVector.size() > 1)
         {
            newMsg->hasPendingMemAccesses = true;
         }

         // the first message should be taken care of first
         AutoDetermineDestSendMsg(newMsg,i->sourceNode,remoteSendTime,
            &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemReadRes","OnDirBlkResp");
         /*
         std::pair<HashMultiMap<Address,LookupData<ReadMsg> >::iterator,
            HashMultiMap<Address,LookupData<ReadMsg> >::iterator> p
            = pendingMainMemAccesses.equal_range(m->addr);
            */
         /*
         // if requesting exclusive, we cannot have more than one item
         DebugAssert(!i->msg->requestingExclusive || tempVector.size()==1);

         for (i++;i < tempVector.end(); i++)
         {
            AutoDetermineDestSendMsg(newMsg,i->sourceNode,remoteSendTime,
            &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemReadRes","OnDirBlkResp");
         }
         */

         // this message might still be in use in OnLocalRead
         //EM().DisposeMsg(pendingMainMemAccesses[m->addr].msg);
         if (pendingMainMemAccessesVector.size()==1)
         {
            pendingMainMemAccesses[m->addr].clear();
            pendingMainMemAccesses.erase(m->addr);
         }
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDirectoryData(m->addr, m->solicitingMessage);
#endif
         EM().DisposeMsg(m);
      }
      /*
      // if we should ignore this intervention
      if (pendingIgnoreInterventions.find(m->addr)!=pendingIgnoreInterventions.end())
      {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
         printDebugInfo("OnRemoteReadResp",*m,"ignoring intervention",src);
#endif
         pendingIgnoreInterventions.erase(m->addr);
         if (pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end())
         {
            EM().DisposeMsg(pendingDirectoryBusySharedReads[m->addr].msg);
            pendingDirectoryBusySharedReads.erase(m->addr);
         }
         else if (pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end())
         {
            EM().DisposeMsg(pendingDirectoryBusyExclusiveReads[m->addr].msg);
            pendingDirectoryBusyExclusiveReads.erase(m->addr);
         }
         return;
      }
      */
      else if (m->isIntervention && pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end() && m->satisfied)
      {// has to be satisfied, because unsatisfied intervention messages are caught at the old owner node that evicted the block
         // or where OnRemoteInvalidateResponse arrived after an OnRemoteRead
         if(m->blockAttached)
         {
            writeToMainMemory(m);
         }
         LookupData<ReadMsg> &ld = pendingDirectoryBusySharedReads[m->addr];
         DebugAssert(directoryData.find(m->addr)!=directoryData.end());
         BlockData &b = directoryData[m->addr];
         
         // added previous owner to sharer in OnDirectoryBlockRequestSharedRead
         //DebugAssert(b.sharers.find(ld.previousOwner)==b.sharers.end());
         //b.sharers.insert(ld.previousOwner);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
         printDebugInfo("OnRemReadRes",*m,("pendDirShRead.erase("
            +to_string<Address>(m->addr)+")").c_str(),src);
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDirectoryData(m->addr, m->solicitingMessage);
#endif
         HandleInterventionComplete(m,false);
      }
      else if (m->isIntervention && pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end() && m->satisfied)
      {// has to be satisfied, because unsatisfied intervention messages are caught at the old owner node that evicted the block
         // or where OnRemoteInvalidateResponse arrived after an OnRemoteRead
         // block should be attached no matter what because we requested from previous owner
         DebugAssert(m->blockAttached);
         if(m->blockAttached)
         {
            writeToMainMemory(m);
         }
         LookupData<ReadMsg> &ld = pendingDirectoryBusyExclusiveReads[m->addr];
         BlockData &b = directoryData[m->addr];
         DebugAssert(b.sharers.size()==0);
         DebugAssert(b.owner==ld.sourceNode);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
         printDebugInfo("OnRemReadRes",*m,("pendDirExRead.erase("
            +to_string<Address>(m->addr)+")").c_str(),src);
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDirectoryData(m->addr, m->solicitingMessage);
#endif
         HandleInterventionComplete(m,true);
      }
      else if (isWaitingForReadResponse)
      {// a local read response could be sent by OnRemoteEviction or here, depending on who satisfies the read
         BlockData &b = directoryData[m->addr];
         // if satisfied, it means OnLocalReadResponse was called before OnLocalEviction
         // if unsatisfied, it means OnLocalEviction was called before OnLocalReadResponse
         // either way, we should not send any messages

         //owner could have been changed by a remote eviction, so there's no way to know what it should be
         //DebugAssert(b.owner!=InvalidNodeID);
         DebugAssert(b.sharers.find(src)==b.sharers.end());
         
         DebugAssert(waitingForReadResponse.find(m->addr)!=waitingForReadResponse.end());
         EM().DisposeMsg(waitingForReadResponse[m->addr]);
         waitingForReadResponse.erase(m->addr);
         EM().DisposeMsg(m);
      }
      else if (!m->satisfied)
      {// a node was read before OnRemoteInvalidateResponse transfers data from the old node
         // to the newer node that was invalidated. send another intervention exclusive request to the previous owner

         BlockData &b = directoryData[m->addr];
         
         // previous owner should have been moved to sharers
         //DebugAssert(isPendingDirectoryExclusiveReads || isPendingDirectorySharedReads);
         // b.owner can be src when m is unsatisfied since this change is made during request
         //DebugAssert(b.owner != src);
         const ReadMsg* read;
         if (isPendingDirectoryExclusiveReads)
         {
            DebugAssert(m->isIntervention);
            LookupData<ReadMsg> &ld = pendingDirectoryBusyExclusiveReads[m->addr];
            DebugAssert(src == ld.previousOwner);
            read = ld.msg;
            DebugAssert(read->originalRequestingNode == ld.sourceNode);
            
            // resend read request
            SendRemoteRead(read,src,"OnRemReadResp");
            
            //AutoDetermineDestSendMsg(read,src,remoteSendTime,&ThreeStageDirectory::OnRemoteRead,
            //   "OnRemReadRes","OnRemRead");
         }
         else if (isPendingDirectorySharedReads)
         {
            DebugAssert(m->isIntervention);
            DebugAssert(b.sharers.find(src)!=b.sharers.end());
            if(b.owner==src)
            {
               return;
            }
            DebugAssert(src == pendingDirectoryBusySharedReads[m->addr].previousOwner);
            read = pendingDirectoryBusySharedReads[m->addr].msg;
            DebugAssert(read->originalRequestingNode == pendingDirectoryBusySharedReads[m->addr].sourceNode);
            //NodeID previousOwner = src;

            // send nak so read request could check the directory when it resends the message
            ReadResponseMsg* rrm = EM().CreateReadResponseMsg(getDeviceID());
            rrm->addr = read->addr;
            rrm->blockAttached = false;
            rrm->directoryLookup = true;
            rrm->satisfied = false;
            rrm->solicitingMessage = read->MsgID();
            rrm->size = read->size;
         
            NodeID sourceNode = pendingDirectoryBusySharedReads[m->addr].sourceNode;
            AutoDetermineDestSendMsg(rrm,sourceNode,remoteSendTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemReadRes","OnDirBlkResp");
            DebugAssert(b.owner==sourceNode || b.sharers.find(sourceNode)!=b.sharers.end());
            DebugAssert(b.owner!=sourceNode || b.sharers.find(sourceNode)==b.sharers.end());

            // erase the newly added node so that we don't keep trying to fetch from an invalid location
            if (b.owner==sourceNode)
            {
               NodeID newOwner = *(b.sharers.begin());
               b.owner = newOwner;
               b.sharers.erase(newOwner);
            }
            else
            {
               b.sharers.erase(sourceNode);
            }

            pendingDirectoryBusySharedReads.erase(m->addr);
            printDirectoryData(m->addr,m->solicitingMessage);

            /*
            AutoDetermineDestSendMsg(read,previousOwner,lookupTime+remoteSendTime,
               &ThreeStageDirectory::OnRemoteRead,"OnRemReadRes","OnRemRead");
               */

            // not doing speculative replies right now
            // request speculative reply from a sharer
            /*
            ReadMsg* speculativeRequest = (ReadMsg*)EM().ReplicateMsg(m);
            speculativeRequest->alreadyHasBlock = false;
            speculativeRequest->directoryLookup = false;
            speculativeRequest->isInterventionShared = true;
            speculativeRequest->requestingExclusive = false;
            speculativeRequest->isInterventionShared = true;
            speculativeRequest->isSpeculative = true;
            AutoDetermineDestSendMsg(speculativeRequest,src,localSendTime,
               &ThreeStageDirectory::OnRemoteSpeculativeReadResponse,"OnDirBlkReqShRead","OnRemoteSpecReadRes");
               */
         }
         else if (b.sharers.size()!=0)
         {//non-busy shared response satisfied. We still have to erase pending directory normal shared read
            BlockData &b = directoryData[m->addr];
            // requesting node doesn't have to be in the directory if eviction was called before OnRemoteReadResponse arrived
            DebugAssert(b.owner==m->originalRequestingNode || b.sharers.find(m->originalRequestingNode)!=b.sharers.end());
            ErasePendingDirectoryNormalSharedRead(m);
            return;
         }
         else
         {
            DebugFail("should not be here");
         }
      }
      else if (directoryData[m->addr].sharers.size()!=0)
      {// regular non-busy shared response
         BlockData &b = directoryData[m->addr];
         // check that the requesting node is in fact in the directory
         // requesting node doesn't have to be in the directory if eviction was called before OnRemoteReadResponse arrived
         //DebugAssert(b.owner==m->originalRequestingNode || b.sharers.find(m->originalRequestingNode)!=b.sharers.end());
         ErasePendingDirectoryNormalSharedRead(m);
         return;
      }
      else if (isWaitingForInvalidationComplete)
      {// we have been invalidated by an exclusive request. erase pending directory normal shared read
         BlockData &b = directoryData[m->addr];
         DebugAssert(b.sharers.size()==0);
         ErasePendingDirectoryNormalSharedRead(m);
         return;
      }
      else
      {
         DebugFail("not supposed to be here");
      }      
	} // OnRemoteReadResponse

   /**
   Assume that speculative reply will arrive faster than shared response or ack because
   the shared response or ack takes two hops as opposed to one hop
   */
   /*
   void ThreeStageDirectory::OnRemoteSpeculativeReadResponse(const BaseMsg* msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_ReadResponse);
      ReadResponseMsg* m = (ReadResponseMsg*)msgIn;
      DebugAssert(m->isSpeculative);
      DebugAssert(pendingSpeculativeReadResponses.find(m->addr)==pendingSpeculativeReadResponses.end());

      pendingSpeculativeReadResponses[m->addr] = m;
   }
   */
   void ThreeStageDirectory::OnRemoteUnrequestedReadResponse(const BaseMsg* msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_UnrequestedReadResponse);
      UnrequestedReadResponseMsg *m = (UnrequestedReadResponseMsg*)msgIn;

      if (reversePendingLocalReads.find(m->addr)!=reversePendingLocalReads.end())
      {// if there are pending reads
         ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];

         HashMap<MessageID,const ReadMsg*> *myHashMap = NULL;
         if (myData.exclusiveRead.size()==1 && m->exclusiveOwnership)
         {
            myHashMap = &myData.exclusiveRead;
            myData.isExclusiveReadSatisfiedByEviction = true;
            if (myData.sharedRead.size()==1)
            {
               myData.isSharedReadSatisfiedByEviction = true;
            }
         }
         else if (myData.sharedRead.size()==1)
         {
            myHashMap = &myData.sharedRead;
            myData.isSharedReadSatisfiedByEviction = true;
         }

         if (myHashMap!=NULL)
         {// if we can satisfy a read
            HashMap<MessageID,const ReadMsg*>::iterator myIterator = myHashMap->begin();

            const ReadMsg* ref = myIterator->second;
         
            ReadResponseMsg *forward = EM().CreateReadResponseMsg(getDeviceID());
            forward->addr = m->addr;
            forward->blockAttached = m->blockAttached;
            forward->evictionMessage = m->evictionMessage;
            forward->exclusiveOwnership = m->exclusiveOwnership;
            forward->satisfied = true;
            forward->size = m->size;
            forward->solicitingMessage = ref->MsgID();
         
		      SendMsg(localConnectionID, forward, satisfyTime + localSendTime);
         } // if (myHashMap!=NULL)
      }// if there are pending reads
      EM().DisposeMsg(m);
      return;
   } // void ThreeStageDirectory::OnRemoteUnrequestedReadResponse(const BaseMsg* msgIn, NodeID src)

	void ThreeStageDirectory::OnRemoteWrite(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Write);
      WriteMsg* m = (WriteMsg*)msgIn;
		DebugAssert(nodeID == directoryNodeCalc->CalcNodeID(m->addr));
		NodeID memoryNode = memoryNodeCalc->CalcNodeID(m->addr);
		if(src != nodeID)
		{
			WriteResponseMsg* wrm = EM().CreateWriteResponseMsg(getDeviceID(), m->GeneratingPC());
			wrm->addr = m->addr;
			wrm->size = m->size;
			wrm->solicitingMessage = m->MsgID();
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNode;
			nm->payloadMsg = m;
			nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = src;
			nm->payloadMsg = wrm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void ThreeStageDirectory::OnRemoteWriteResponse(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_WriteResponse);
      WriteResponseMsg* m = (WriteResponseMsg*)msgIn;
      DebugAssert(pendingMemoryWrites.find(m->solicitingMessage)!=pendingMemoryWrites.end());
      pendingMemoryWrites.erase(m->solicitingMessage);
		EM().DisposeMsg(m);
	}
	void ThreeStageDirectory::OnRemoteEviction(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Eviction);
      EvictionMsg* m = (EvictionMsg*)msgIn;
      Address myAddress = m->addr;
      MessageID myMessageID = m->MsgID();
		DebugAssert(directoryData.find(m->addr) != directoryData.end());
		BlockData& b = directoryData[m->addr];

      /*
      TODO
      2010/10/14 Eric
      if (m->isBlockNotFound)
      {
         if (b.owner==src)
         {
            if (b.sharers.find(src)==b.sharers.end())
            {
               b.owner = InvalidNodeID;
            }
            else
            {
               NodeID newOwner = *b.sharers.begin();
               b.sharers.erase(newOwner);
               b.owner = newOwner;
            }
         }
         else if (b.sharers.find(src)!=b.sharers.end())
         {
            b.sharers.erase(src);
         }
         EM().DisposeMsg(m);
      }
      else*/
         if (pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end())
      {// if directory state is Busy-shared, transitions to shared, a shared response is returned to
         // the owner marked in the directory. A writeback busy acknowledgement is also sent to the requestor
         
         DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)==pendingDirectoryBusyExclusiveReads.end());
         // block doesn't have to be attached because the block could be in Exclusive state.
            // block will be attached if block is in Own or Modified state
         //DebugAssert(m->blockAttached);

         LookupData<ReadMsg> &ld = pendingDirectoryBusySharedReads[m->addr];
         const ReadMsg* ref = ld.msg;
         DebugAssert(ref);
                
         // src is either in owner or in sharers
         DebugAssert(b.sharers.find(src)!=b.sharers.end() || b.owner==src);
         DebugAssert(b.sharers.find(src)==b.sharers.end() || b.owner!=src);
         // delete share
         NodeID newOwner = InvalidNodeID;  
         if (b.owner==src)
         {
            // have to dereference the iterator
            newOwner = *b.sharers.begin();
            b.owner = newOwner;
            b.sharers.erase(newOwner);
         }
         else
         {
            b.sharers.erase(src);
         }

         // if we just deleted a share, we should send a sharedResponse
         //if (newOwner==InvalidNodeID)
         {
            // send shared response to the owner marked in the directory
            DebugAssert(b.owner!=InvalidNodeID);
            ReadResponseMsg *sharedResponse = EM().CreateReadResponseMsg(getDeviceID());
            sharedResponse->addr = m->addr;
            // TODO changed this to true because in this protocol, eviction is the same as writeback,
               // so we have to forward the writeback to the next owner
            //sharedResponse->blockAttached = m->blockAttached;
            sharedResponse->blockAttached = true;
            sharedResponse->directoryLookup = true;
            sharedResponse->exclusiveOwnership = false;
            //sharedResponse->isFromEviction = true;
            sharedResponse->evictionMessage = m->MsgID();
            sharedResponse->isIntervention = false;
            sharedResponse->originalRequestingNode = src;
            sharedResponse->size = m->size;
            sharedResponse->satisfied = true;
            sharedResponse->solicitingMessage = ref->MsgID();
            AutoDetermineDestSendMsg(sharedResponse,b.owner,remoteSendTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemEvic","OnDirBlkResp");
         }
         // else if we deleted the owner, it means the owner just evicted itself,
            // so we should not send shared response

         // send writeback busy acknowledgement to eviction requestor
         EvictionResponseMsg *busyAck = EM().CreateEvictionResponseMsg(getDeviceID());
         busyAck->addr = m->addr;
         busyAck->isBusyAck = true;
         busyAck->isExclusive = false;
         busyAck->size = m->size;
         busyAck->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(busyAck,src,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionResponse,"OnRemEvic","OnRemEvicRes");

         // if we are here, it means OnLocalEviction is called before OnRemoteRead called on the oldOwner,
         // so we are waiting for a read response message that could be satisfied or unsatisfied
         DebugAssert(waitingForReadResponse.find(m->addr)==waitingForReadResponse.end());
         waitingForReadResponse[m->addr] = m;

         // transitions from busy-shared to shared even if the eviction contains no data
            // waitingForReadResponse will keep the directory busy and prevent messages from
            // requesting data from nodes that don't have them
         //if (pendingDirectoryBusySharedReads[m->addr].msg->alreadyHasBlock || m->blockAttached)
         // for some reason, this message seems to be in use, still
         //EM().DisposeMsg(pendingDirectoryBusySharedReads[m->addr].msg);
         pendingDirectoryBusySharedReads.erase(m->addr);
      }
      else if (pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end())
      {
         // if directory state is Busy-exclusive, transitions to Exclusive,
            // an exclusive response is returned to the owner marked in the directory
         // A writeback busy acknowledgement is also sent to the requestor
         DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)==pendingDirectoryBusySharedReads.end());
         LookupData<ReadMsg> &ld = pendingDirectoryBusyExclusiveReads[m->addr];
         const ReadMsg *ref = ld.msg;
         DebugAssert(ref);
         DebugAssert(b.sharers.size()==0);
         // block doesn't have to be attached because the block could be in Exclusive state.
            // block will be attached if block is in Own or Modified state
         //DebugAssert(m->blockAttached);
         DebugAssert(src==ld.previousOwner);
         //DebugAssert(src==b.owner);

         // send exclusive response to the owner marked in the directory
         DebugAssert(b.owner!=InvalidNodeID);
         ReadResponseMsg *exclusiveResponse = EM().CreateReadResponseMsg(getDeviceID());
         exclusiveResponse->addr = m->addr;
         // changed this to true because in this protocol, eviction is the same as writeback,
            // and we have to send the writeback to the new owner
         //exclusiveResponse->blockAttached = m->blockAttached;
         exclusiveResponse->blockAttached = true;
         exclusiveResponse->directoryLookup = true;
         exclusiveResponse->exclusiveOwnership = true;
         //exclusiveResponse->isFromEviction = true;
         exclusiveResponse->evictionMessage = m->MsgID();
         exclusiveResponse->isIntervention = false;
         exclusiveResponse->originalRequestingNode = src;
         exclusiveResponse->size = m->size;
         exclusiveResponse->solicitingMessage = ref->MsgID();
         AutoDetermineDestSendMsg(exclusiveResponse,b.owner,remoteSendTime,
            &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemEvic","OnDirBlkResp");

         // send writeback busy acknowledgement to eviction requestor
         EvictionResponseMsg *busyAck = EM().CreateEvictionResponseMsg(getDeviceID());
         busyAck->addr = m->addr;
         busyAck->isBusyAck = true;
         busyAck->isExclusive = true;
         busyAck->size = m->size;
         busyAck->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(busyAck,src,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionResponse,"OnRemEvic","OnRemEvicRes");

         // if we are here, it means OnLocalEviction is called before OnRemoteRead called on the oldOwner,
         // so we are waiting for an unsatisfied read response message
         DebugAssert(waitingForReadResponse.find(m->addr)==waitingForReadResponse.end());
         waitingForReadResponse[m->addr] = m;

         // transitions from busy-exclusive to exclusive
         EM().DisposeMsg(pendingDirectoryBusyExclusiveReads[m->addr].msg);
         pendingDirectoryBusyExclusiveReads.erase(m->addr);
      }
      else if (b.owner==src && b.sharers.size()==0)
      {// if directory state is Exclusive with requestor as owner,
         // transitions to Unowned and returns a writeback exclusive acklowledge
         // to the requestor
         DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)==pendingDirectoryBusySharedReads.end());
         DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)==pendingDirectoryBusyExclusiveReads.end());
         EvictionResponseMsg* rm = EM().CreateEvictionResponseMsg(getDeviceID(),m->GeneratingPC());
			rm->addr = m->addr;
         rm->isExclusive = true;
			rm->size = m->size;
			rm->solicitingMessage = m->MsgID();

         AutoDetermineDestSendMsg(rm,src,remoteSendTime,&ThreeStageDirectory::OnRemoteEvictionResponse
            ,"OnRemEvic","OnRemEvicResponse");

         b.owner=InvalidNodeID;
         EM().DisposeMsg(m);
      }
      // TODO 2010/09/23 Eric
      // need this because shared blocks can be evicted, too
      // if src is in sharer, remove src from sharer
      else
		{
         // check that src is actually in directoryData
         //DebugAssert(b.owner==src || b.sharers.find(src)!=b.sharers.end());
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         if (b.owner==src || b.sharers.find(src)!=b.sharers.end())
         {
            printDebugInfo("OnRemEvic",*m,
               ("b.sharers.erase("+to_string<NodeID>(Memory::convertNodeIDToDeviceID(src))+")").c_str(),src);
         }
#endif         
         if (b.owner==src)
         {
            // remove src and move the top of sharers into owner
            DebugAssert(b.sharers.size()>0);
            HashSet<NodeID>::iterator myIterator = b.sharers.begin();
            b.owner = *myIterator;
            b.sharers.erase(b.owner);
         }
         else
         {
            b.sharers.erase(src);
         }

         // send unrequested read response to owner in case there's no more data blocks floating around
         //if (b.sharers.size()==0)
         {
            UnrequestedReadResponseMsg *forward = EM().CreateUnrequestedReadResponseMsg(getDeviceID());
            forward->addr = m->addr;
            forward->blockAttached = true;
            forward->evictionMessage = m->MsgID();
            if (b.sharers.size()==0)
            {
               forward->exclusiveOwnership = true;
            }
            forward->size = m->size;
            
            AutoDetermineDestSendMsg(forward,b.owner,remoteSendTime,
               &ThreeStageDirectory::OnRemoteUnrequestedReadResponse,"OnRemEvic","OnRemUnreqReadRes");
         }

         // send regular eviction response msg
         EvictionResponseMsg *evictionResponse = EM().CreateEvictionResponseMsg(getDeviceID());
         evictionResponse->addr = m->addr;
         // I think exclusivity doesn't matter in a regular eviction 2010/09/28 Eric
         //evictionResponse->isExclusive = 
         evictionResponse->size = m->size;
         evictionResponse->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(evictionResponse,src,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionResponse,"OnRemEvic","OnRemEvicRes");         
         EM().DisposeMsg(m);
		}

      // if block is attached, write back to memory
		if(m->blockAttached)
		{
			writeToMainMemory(m);
		}
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(myAddress, myMessageID);
#endif
	} // ThreeStageDirectory::OnRemoteEviction

	void ThreeStageDirectory::OnRemoteEvictionResponse(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_EvictionResponse);
      EvictionResponseMsg* m = (EvictionResponseMsg*)msgIn;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_EVICTION
      printDebugInfo("OnRemEvicResponse", *m,
            ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str());
#endif
      // if we have OnLocalReads waiting for this evictionResponse
      if (waitingForEvictionResponse.find(m->addr)!=waitingForEvictionResponse.end())
      {
         SendDirectoryBlockRequest(waitingForEvictionResponse[m->addr]);
         waitingForEvictionResponse.erase(m->addr);
      }
      if (m->isBusyAck)
      {// this node wouldn't know that this eviction is coming because a busy ack is sent from the directory
         //DebugAssert(waitingForEvictionBusyAck.find(m->addr)!=waitingForEvictionBusyAck.end());
         //EM().DisposeMsg(waitingForEvictionBusyAck[m->addr]);
         //waitingForEvictionBusyAck.erase(m->addr);
		   DebugAssert(pendingEviction.find(m->addr) != pendingEviction.end());
         EM().DisposeMsg(pendingEviction[m->addr]);
		   pendingEviction.erase(m->addr);
		   SendMsg(localConnectionID, m, localSendTime);
      }
      else
      {
		   DebugAssert(pendingEviction.find(m->addr) != pendingEviction.end());
         EM().DisposeMsg(pendingEviction[m->addr]);
		   pendingEviction.erase(m->addr);
		   SendMsg(localConnectionID, m, localSendTime);
      }
	}

   void ThreeStageDirectory::OnRemoteEvictionBusyAck(const BaseMsg* msgIn, NodeID src)
	{
      DebugFail("should not be here");
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_EvictionBusyAck);
      EvictionBusyAckMsg* m = (EvictionBusyAckMsg*)msgIn;
      // the following should not be true because this node should not be the directory
      DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)==pendingDirectoryBusySharedReads.end());
      DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)==pendingDirectoryBusyExclusiveReads.end());

      DebugAssert(pendingEviction.find(m->addr) != pendingEviction.end());
		pendingEviction.erase(m->addr);

      DebugAssert(waitingForEvictionBusyAck.find(m->addr)!=waitingForEvictionBusyAck.end());
      EM().DisposeMsg(waitingForEvictionBusyAck[m->addr]);
      waitingForEvictionBusyAck.erase(m->addr);
      EM().DisposeMsg(m);
	}

	void ThreeStageDirectory::OnRemoteInvalidate(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Invalidate);
      InvalidateMsg* m = (InvalidateMsg*)msgIn;
      DebugAssert(m->newOwner != InvalidNodeID);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
      printDebugInfo("OnRemoteInvalidate", *m,
            ("PRI.insert("+to_string<Address>(m->addr)+")").c_str());
#endif
      DebugAssert(pendingRemoteInvalidates.find(m->MsgID()) == pendingRemoteInvalidates.end());
		pendingRemoteInvalidates[m->MsgID()].sourceNode = src;
		pendingRemoteInvalidates[m->MsgID()].msg = m;
		SendMsg(localConnectionID, EM().ReplicateMsg(m), localSendTime);
	}

	void ThreeStageDirectory::OnRemoteInvalidateResponse(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_InvalidateResponse);
      InvalidateResponseMsg* m = (InvalidateResponseMsg*)msgIn;
      // the following is no longer true because this method is being used by the
         // requester instead of the directory now
      //DebugAssert(directoryData.find(m->addr) != directoryData.end());
#if defined MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA && !defined _WIN32
      #define MEMORY_3_STAGE_DIRECTORY_DEBUG_ARRAY_SIZE 20
      BlockData& b = directoryData[m->addr];
      NodeID sharers[MEMORY_3_STAGE_DIRECTORY_DEBUG_ARRAY_SIZE];
      b.sharers.convertToArray(sharers,MEMORY_3_STAGE_DIRECTORY_DEBUG_ARRAY_SIZE);
#endif
      // invalidateResponse could arrive before directoryBlockResponse
      //DebugAssert(waitingForInvalidates.find(m->addr)!=waitingForInvalidates.end());

      // increment the pending invalidates counter. If it is the same as the number of invalidates
         // we are waiting for, it means all
         // invalidates have been accounted for, so we can send the read response now.
      InvalidateData &ld = waitingForInvalidates[m->addr];
      ld.invalidatesReceived++;
      // if we have received all the invalidates
      if (ld.invalidatesReceived==ld.invalidatesWaitingFor)
      {
         HandleReceivedAllInvalidates(m->addr);
      } // if (waitingForInvalidates[m->addr].count==0)
      // did all the directoryData operations in OnDirectoryBlockRequest      
		if(m->blockAttached)
		{
			writeToMainMemory(m);
		}
      EM().DisposeMsg(m);
      // the following are no longer true because we should be in requester, not directory
      //DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)==pendingDirectoryBusyExclusiveReads.end());
      //DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)==pendingDirectoryBusySharedReads.end());
      return;
	}  // OnRemoteInvalidateResponse

   void ThreeStageDirectory::OnRemoteMemAccessComplete(const BaseMsg* msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_MemAccessComplete);
      MemAccessCompleteMsg* m = (MemAccessCompleteMsg*) msgIn;
      DebugAssert(m->addr != 0);

      DebugAssert(directoryNodeCalc->CalcNodeID(m->addr)==nodeID);
      BlockData &b = directoryData[m->addr];

      DebugAssert(pendingMainMemAccesses.find(m->addr)!=pendingMainMemAccesses.end());
      std::vector<LookupData<ReadMsg> > pendingMainMemAccessesVector = pendingMainMemAccesses[m->addr];
      DebugAssert(pendingMainMemAccessesVector.size()>1);
         
      // used for indexing which message inside pendingMainMemAccessesVector we are in
      int messageIndex = 0;
      // do not start at begin, because we already sent the first message out
      // received data from memory, so allow previously queued up messages to go through
      for (std::vector<LookupData<ReadMsg> >::iterator i = pendingMainMemAccessesVector.begin()+1;
         i < pendingMainMemAccessesVector.end(); i++)
      {
         // b.owner can be i->sourceNode if a shared read triggers mem access but a exclusive read follows that
         //DebugAssert(b.owner!=i->sourceNode);
         // check if sourceNode is in sharers. It might not be in sharers if it is first request
         DebugAssert(b.sharers.find(i->sourceNode)==b.sharers.end());

         CBOnDirectoryBlockRequest::FunctionType* f = cbOnDirectoryBlockRequest.Create();
         f->Initialize(this,i->msg,i->sourceNode);
         // stagger the events so they don't all try to request at the same time
         EM().ScheduleEvent(f, (satisfyTime+localSendTime) * messageIndex);
         // call request immediately
         //EM().ScheduleEvent(f, 0);

         //OnDirectoryBlockRequest(i->msg,i->sourceNode);
         /*
         //2010/09/30 Eric
         // switch from sending unsatisfied to forwarding OnLocalRead messages
         // create unsatisfied to make them resend the message
         ReadResponseMsg* response = EM().CreateReadResponseMsg(getDeviceID());
         response->addr = i->msg->addr;
         response->blockAttached = false;
         response->directoryLookup = true;
         response->exclusiveOwnership = false;
         response->isIntervention = false;
         response->originalRequestingNode = i->sourceNode;
         response->satisfied = false;
         response->size = i->msg->size;
         response->solicitingMessage = i->msg->MsgID();
         AutoDetermineDestSendMsg(response,i->sourceNode,remoteSendTime+lookupTime,
            &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemMemAccCom","OnDirBlkResp");
         */
      }

      // this message might still be in use in OnLocalRead
      //EM().DisposeMsg(pendingMainMemAccesses[m->addr].msg);
      pendingMainMemAccesses[m->addr].clear();
      pendingMainMemAccesses.erase(m->addr);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr, m->MsgID());
#endif
      EM().DisposeMsg(m);
   }

   void ThreeStageDirectory::OnRemoteInterventionComplete(const BaseMsg* msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_InterventionComplete);
      InterventionCompleteMsg* m = (InterventionCompleteMsg*)msgIn;
      DebugAssert(directoryNodeCalc->CalcNodeID(m->addr)==nodeID);

      bool isPendingDirectorySharedReads = (pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end());
      bool isPendingDirectoryExclusiveReads = (pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end());
      bool isBusy = (isPendingDirectorySharedReads || isPendingDirectoryExclusiveReads);
      DebugAssert(!isPendingDirectorySharedReads || !isPendingDirectoryExclusiveReads);

      if (isBusy)
      {
         HandleInterventionComplete(m,isPendingDirectoryExclusiveReads);
      }
      else
      {// else not busy means that the message to directory arrived before message to newOwner

         // if we are still waiting for intervention complete, it means that pendingSharedReads has already
            // been cleared by OnRemoteReadResponse
         if (waitingForInterventionComplete.find(m->solicitingMessage)!=waitingForInterventionComplete.end())
         {
            DebugAssert(waitingForInterventionComplete[m->solicitingMessage].rrm!=NULL);
            waitingForInterventionComplete.erase(m->solicitingMessage);
            if (waitingForReadResponse.find(m->addr)!=waitingForReadResponse.end())
            {
               EM().DisposeMsg(waitingForReadResponse[m->addr]);
               waitingForReadResponse.erase(m->addr);
            }
            EM().DisposeMsg(m);
         }
      }
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr, m->solicitingMessage);
#endif
   }

   void ThreeStageDirectory::OnRemoteInvalidationComplete(const BaseMsg* msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_InvalidationComplete);
      InvalidationCompleteMsg* m = (InvalidationCompleteMsg*)msgIn;

      bool isWaitingForInvalidationComplete = (waitingForInvalidationComplete.find(m->addr)!=waitingForInvalidationComplete.end());
      DebugAssert(directoryNodeCalc->CalcNodeID(m->addr)==nodeID);
      DebugAssert(isWaitingForInvalidationComplete);

      BlockData &b = directoryData[m->addr];
      LookupData<ReadMsg> &ld = waitingForInvalidationComplete[m->addr];
      /*
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
      printDebugInfo("OnDirBlkRequ",*m,"OnRemInvCom",src);
#endif
      */
      // send nak to make it resend request
      //OnDirectoryBlockRequest(ld.msg,ld.sourceNode);
      /*
      const ReadMsg *ref = ld.msg;
      ReadResponseMsg *response = EM().CreateReadResponseMsg(getDeviceID());
      response->addr = ref->addr;
      response->blockAttached = false;
      response->directoryLookup = true;
      response->satisfied = false;
      response->size = ref->size;
      response->solicitingMessage = ref->MsgID();
      AutoDetermineDestSendMsg(response,ld.sourceNode,remoteSendTime,
         &ThreeStageDirectory::OnDirectoryBlockResponse,"OnRemInvCom","OnDirBlkResp");
         */

      // don't delete ref because it's still in OnLocalRead
      waitingForInvalidationComplete.erase(m->addr);
      EM().DisposeMsg(m);
   }
   
   void ThreeStageDirectory::OnRemoteReadComplete(const BaseMsg* msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_ReadComplete);
      ReadCompleteMsg* m = (ReadCompleteMsg*)msgIn;

      DebugAssert(directoryNodeCalc->CalcNodeID(m->addr)==nodeID);

      DebugAssert(invalidateLock.find(m->addr)!=invalidateLock.end());
      std::vector<ReadMsg> &invalidateLockVector = invalidateLock[m->addr];

      bool hasMessage = false;
      std::vector<ReadMsg>::iterator i;
      for (i = invalidateLockVector.begin(); i < invalidateLockVector.end(); i++)
      {
         if (i->MsgID() == m->solicitingMessage)
         {
            hasMessage = true;
            break;
         }
      }
      DebugAssert(hasMessage);

      ReadMsg myReadMsg = *i;
      // cannot delete myReadMsg because it is in OnLocalRead
      //EM().DisposeMsg(&myReadMsg);
      invalidateLockVector.erase(i);
      if (invalidateLockVector.size()==0)
      {
         invalidateLock.erase(m->addr);
      }
      EM().DisposeMsg(m);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr,m->solicitingMessage);
#endif
   } // ThreeStageDirectory::OnRemoteReadComplete

	void ThreeStageDirectory::OnDirectoryBlockRequest(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
      DebugAssert(m->directoryLookup==true);
		DebugAssert(directoryNodeCalc->CalcNodeID(m->addr)==nodeID);
      
      bool isInPendingDirectorySharedReads = (pendingDirectoryBusySharedReads.find(m->addr)!=pendingDirectoryBusySharedReads.end());
      bool isInPendingDirectoryExclusiveReads = (pendingDirectoryBusyExclusiveReads.find(m->addr)!=pendingDirectoryBusyExclusiveReads.end());
      bool isWaitingForInterventionComplete = (waitingForInterventionComplete.find(m->addr)!=waitingForInterventionComplete.end());
      bool isWaitingForInvalidationComplete = (waitingForInvalidationComplete.find(m->addr)!=waitingForInvalidationComplete.end());
      bool isWaitingForReadResponse = (waitingForReadResponse.find(m->addr)!=waitingForReadResponse.end());
      bool isDirectoryBusy = isInPendingDirectorySharedReads || isInPendingDirectoryExclusiveReads || isWaitingForInterventionComplete
         || isWaitingForInvalidationComplete || isWaitingForReadResponse;
      if (isDirectoryBusy)
		{//cannot complete the request at this time
			if(src == nodeID)
			{
				CBOnDirectoryBlockRequest::FunctionType* f = cbOnDirectoryBlockRequest.Create();
				f->Initialize(this,m,src);
				EM().ScheduleEvent(f,lookupRetryTime);
			}
			else
			{
				NetworkMsg* n = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
				ReadResponseMsg* rm = EM().CreateReadResponseMsg(getDeviceID(),m->GeneratingPC());
				n->sourceNode = nodeID;
				n->destinationNode = src;
				n->payloadMsg = rm;
				rm->addr = m->addr;
				rm->satisfied = false;
				rm->size = m->size;
				rm->solicitingMessage = m->MsgID();
				rm->blockAttached = false;
				rm->directoryLookup = true;
				rm->originalRequestingNode = m->originalRequestingNode;
				SendMsg(remoteConnectionID, n, lookupTime + remoteSendTime);
				EM().DisposeMsg(m);
			}
		} // endif cannot satisfy request at this time
      else if (pendingMainMemAccesses.find(m->addr)!=pendingMainMemAccesses.end())
      {// if we are waiting for memory access to come back
         LookupData<ReadMsg> ld;
         ld.msg = m;
         ld.sourceNode = src;
         pendingMainMemAccesses[m->addr].push_back(ld);
         BlockData &b = directoryData[m->addr];
         // b.owner can be src if a sharedRead activated mem access and a exclusiveRead follows that
         DebugAssert(b.sharers.find(src)==b.sharers.end());
      }
      else if (!m->requestingExclusive)
      {// if we are doing a shared read request
         OnDirectoryBlockRequestSharedRead(m,src);
      }
      else // m->requestingExclusive
      {
         OnDirectoryBlockRequestExclusiveRead(m,src);
      } //else m->requestingExclusive
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
      printDirectoryData(m->addr, m->MsgID());
#endif
	}

   void ThreeStageDirectory::OnDirectoryBlockRequestSharedRead(const BaseMsg *msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
      DebugAssert(!m->requestingExclusive);

      BlockData &b = directoryData[m->addr];
      // if directory state is Unowned or Exclusive with requester as owner,
      // transitions to Exclusive and returns an exclusive reply to the requester
      if ((b.owner==InvalidNodeID&&b.sharers.size()==0) || (b.owner==src&&b.sharers.size()==0))
      {
         // perform directory fetch before setting b.owner
         if (b.owner==InvalidNodeID || (b.owner==src&&!m->alreadyHasBlock))
         {// if directory is unowned OR it is owned by src but it doesn't have the block
            // fetch from memory
            LookupData<ReadMsg> ld;
            ld.msg = m;
            ld.sourceNode = src;
            DebugAssert(pendingMainMemAccesses.find(m->addr)==pendingMainMemAccesses.end());
            pendingMainMemAccesses[m->addr].push_back(ld);
            //pendingMainMemAccesses.insert(HashMap<Address,LookupData<ReadMsg> >::value_type(m->addr,ld));
            PerformDirectoryFetch(m,src,true,memoryNodeCalc->CalcNodeID(m->addr));
         }
         else if (b.owner==src && m->alreadyHasBlock)
         {// if owner is src and it already has block, then just send exclusive reply
            DebugAssert(m->alreadyHasBlock);
            // send directoryBlockResponse
            ReadResponseMsg* forward = EM().CreateReadResponseMsg(getDeviceID());
            forward->addr = m->addr;
            forward->blockAttached = false;
            forward->directoryLookup = true;
            forward->exclusiveOwnership = true;
            forward->satisfied = true;
            forward->size = m->size;
            forward->solicitingMessage = m->MsgID();

            AutoDetermineDestSendMsg(forward,src,lookupTime+remoteSendTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkReqShRead","OnDirBlkResp");
         } // else if (b.owner==src)
         else
         {
            DebugFail("b.owner has to be src or InvalidNodeID");
         }
         b.owner = src;
         // do not dispose m here, because the original OnLocalRead still has it
         //EM().DisposeMsg(m);
         return;
      } // if ((b.owner==InvalidNodeID&&b.sharers.size()==0) || (b.owner==src&&b.sharers.size()==0))
      else if (b.sharers.size()!=0)
      {// if directory state is shared, add requesting node to sharer and request reply from any sharer
         bool hasSentRequestPreviously = false;
         AddrLookupIteratorPair iteratorPair = pendingDirectoryNormalSharedReads.equal_range(m->addr);
         for (; iteratorPair.first != iteratorPair.second; ++iteratorPair.first)
         {
            LookupData<ReadMsg> &ld = iteratorPair.first->second;
            const ReadMsg *readMsg = ld.msg;
            if (m->MsgID()==readMsg->MsgID())
            {
               hasSentRequestPreviously = true;
               break;
            }
            DebugAssert(m->MsgID() != readMsg->MsgID());
         }
         if (!hasSentRequestPreviously)
         {
            // puts a lock to prevent invalidates from sending before the data arrived at new sharer
            std::vector<ReadMsg> &invalidateLockVector = invalidateLock[m->addr];
            invalidateLockVector.push_back(*m);

            AddPendingDirectoryNormalSharedRead(m,src);

            // perform directory fetch before modifying directoryData
            ReadMsg *forward = (ReadMsg*)EM().ReplicateMsg(m);
            forward->isWaitingForInvalidateUnlock = true;
            forward->isNonBusySharedRead = true;
            
            // find a destination that is not src, because if src is in
               // b, it means we just inserted it and we haven't satisfied the insertion
            NodeID dest = InvalidNodeID;
            if (b.owner!=src)
            {
               DebugAssert(b.owner!=InvalidNodeID);
               dest = b.owner;
            }
            else
            {
               /*
               for each (NodeID tempNodeID in b.sharers)
               {
                  if (tempNodeID != src)
                  {
                     dest = tempNodeID;
                     break;
                  }
               }
               */
               for (HashSet<NodeID>::iterator i = b.sharers.begin(); i != b.sharers.end(); i++)
               {
                  NodeID tempNodeID = *i;
                  if (tempNodeID != src)
                  {
                     dest = tempNodeID;
                     break;
                  }
               }
            }
            DebugAssert(dest!=InvalidNodeID);
            PerformDirectoryFetch(forward,src,false,dest);
         
            if (b.owner!=src && b.sharers.find(m->addr)==b.sharers.end())
            {
               // add src to sharers if not already in it
               b.sharers.insert(src);
            }
            // do not dispose msg here, because we are forwarding it
            //EM().DisposeMsg(m);
            return;
         }
         else
         {//hasSentRequestPreviously
            DebugAssert(IsInPendingDirectoryNormalSharedRead(m));
            std::vector<ReadMsg> &invalidateLockVector = invalidateLock[m->addr];
            bool hasMessage = false;
            for (std::vector<ReadMsg>::iterator i=invalidateLockVector.begin();
               i < invalidateLockVector.end(); i++)
            {
               if (i->MsgID()==m->MsgID())
               {
                  hasMessage = true;
                  break;
               }
            }
            DebugAssert(hasMessage);

            ReadMsg *forward = (ReadMsg*)EM().ReplicateMsg(m);
            forward->isWaitingForInvalidateUnlock = true;
            forward->isNonBusySharedRead = true;
            //PerformDirectoryFetch(forward,src,false,b.owner);

            // find a destination that is not src, because if src is in
               // b, it means we just inserted it and we haven't satisfied the insertion
            NodeID dest = InvalidNodeID;
            if (b.owner!=src)
            {
               DebugAssert(b.owner!=InvalidNodeID);
               dest = b.owner;
            }
            else
            {/*
               for each (NodeID tempNodeID in b.sharers)
               {
                  if (tempNodeID != src)
                  {
                     dest = tempNodeID;
                     break;
                  }
               }
               */
               for (HashSet<NodeID>::iterator i = b.sharers.begin(); i != b.sharers.end(); i++)
               {
                  NodeID tempNodeID = *i;
                  if (tempNodeID != src)
                  {
                     dest = tempNodeID;
                     break;
                  }
               }
            }
            DebugAssert(dest!=InvalidNodeID);
            PerformDirectoryFetch(forward,src,false,dest);
            // cannot dispose this message
            //EM().DisposeMsg(m);
            return;            
         }
      }// else if (b.sharers.size()!=0)
      else if (b.owner!=InvalidNodeID&&b.owner!=src)
      {// if directory state is Exclusive with another owner, transitions to Busy-shared with
         // requestor as owner and send out an intervention shared request to the previous
         // owner and a speculative reply to the requestor
         DebugAssert(pendingDirectoryBusySharedReads.find(m->addr)==pendingDirectoryBusySharedReads.end());
         DebugAssert(b.sharers.size()==0);

         // send intervention shared request to the previous owner
         NodeID previousOwner = b.owner;
         ReadMsg* read = (ReadMsg*)EM().ReplicateMsg(m);
         read->alreadyHasBlock = false;
         read->directoryLookup = false;
         read->isIntervention = true;
         read->requestingExclusive = false;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
         printDebugInfo("OnDirBlkReqShRead",*m,("pendDirShRead.insert("
            +to_string<Address>(m->addr)+")").c_str(),src);
#endif
         pendingDirectoryBusySharedReads[m->addr].sourceNode = src;
         pendingDirectoryBusySharedReads[m->addr].msg = read;
         pendingDirectoryBusySharedReads[m->addr].previousOwner = previousOwner;
         AutoDetermineDestSendMsg(read,previousOwner,localSendTime,
            &ThreeStageDirectory::OnRemoteRead,"OnDirBlkReqShRead","OnRemRead");

         // not doing speculative replies right now
         // request speculative reply from a sharer
         /*
         ReadMsg* speculativeRequest = (ReadMsg*)EM().ReplicateMsg(m);
         speculativeRequest->alreadyHasBlock = false;
         speculativeRequest->directoryLookup = false;
         speculativeRequest->isInterventionShared = true;
         speculativeRequest->requestingExclusive = false;
         speculativeRequest->isInterventionShared = true;
         speculativeRequest->isSpeculative = true;
         AutoDetermineDestSendMsg(speculativeRequest,src,localSendTime,
            &ThreeStageDirectory::OnRemoteSpeculativeReadResponse,"OnDirBlkReqShRead","OnRemoteSpecReadRes");
            */

         b.owner = src;
         // previousOwner should be added to sharers when we received reply from previous owner, not here
         DebugAssert(b.sharers.find(previousOwner)==b.sharers.end());
         b.sharers.insert(previousOwner);
         // do not dispose m here, because the original OnLocalRead still has it
         //EM().DisposeMsg(m);
         return;
      } // else if (b.owner!=InvalidNodeID&&b.owner!=src)
      else
      {
         DebugFail("Should not be here");
         return;
      }
   } // OnDirectoryBlockRequestSharedRead()

   void ThreeStageDirectory::OnDirectoryBlockRequestExclusiveRead(const BaseMsg *msgIn, NodeID src)
   {
      DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
      DebugAssert(m->requestingExclusive);

      BlockData &b = directoryData[m->addr];
      
      // if directory state is Unowned or Exclusive with requester as owner,
      // transitions to Exclusive and returns an exclusive reply to the requester
      if ((b.owner==InvalidNodeID&&b.sharers.size()==0) || (b.owner==src&&b.sharers.size()==0))
      {
         // perform directory fetch before setting b.owner
         if (b.owner==InvalidNodeID || (b.owner==src&&!m->alreadyHasBlock))
         {// if directory is unowned OR it is owned by src but it doesn't have the block
            // fetch from memory
            LookupData<ReadMsg> ld;
            ld.msg = m;
            ld.sourceNode = src;
            DebugAssert(pendingMainMemAccesses.find(m->addr)==pendingMainMemAccesses.end());
            pendingMainMemAccesses[m->addr].push_back(ld);
            PerformDirectoryFetch(m,src,true,memoryNodeCalc->CalcNodeID(m->addr));
         }
         else if (b.owner==src)
         {// if owner is src and it already has block, then just send exclusive reply
            DebugAssert(m->alreadyHasBlock);
            // send directoryBlockResponse
            ReadResponseMsg* forward = EM().CreateReadResponseMsg(getDeviceID());
            forward->addr = m->addr;
            forward->blockAttached = false;
            forward->directoryLookup = true;
            forward->exclusiveOwnership = true;
            forward->satisfied = true;
            forward->size = m->size;
            forward->solicitingMessage = m->MsgID();

            AutoDetermineDestSendMsg(forward,src,lookupTime+remoteSendTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkReqExRead","OnDirBlkResp");
         } // else if (b.owner==src)
         else
         {
            DebugFail("b.owner has to be src or InvalidNodeID");
         }
         b.owner = src;
         // do not dispose m here, because it is still in pendingLocalReads
         //EM().DisposeMsg(m);
         return;
      } // if ((b.owner==InvalidNodeID&&b.sharers.size()==0) || (b.owner==src&&b.sharers.size()==0))
      //else if (b.owner==InvalidNodeID && b.sharers.size()!=0)
      // b.owner could be source, so we cannot assume that b.owner has to be InvalidNodeID
      else if (b.sharers.size()!=0)
      {// if directory state is shared, transition to Exclusive and a exclusive reply with invalidates pending
         // is returned to the requestor. Invalidations are sent to the sharers.

         // if the data nodes are still waiting for their data to arrive, we cannot invalidate them, so send nak
         if (invalidateLock.find(m->addr)!=invalidateLock.end())
         {
            ReadResponseMsg *rrm = EM().CreateReadResponseMsg(getDeviceID());
            rrm->addr = m->addr;
            rrm->directoryLookup = true;
            rrm->satisfied = false;
            rrm->size = m->size;
            rrm->solicitingMessage = m->MsgID();

            AutoDetermineDestSendMsg(rrm, src, remoteSendTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkExRead","OnDirBlkResp");
            return;
         }

         DebugAssert(waitingForInvalidationComplete.find(m->addr)==waitingForInvalidationComplete.end());
         waitingForInvalidationComplete[m->addr].msg = m;
         waitingForInvalidationComplete[m->addr].sourceNode = src;

         int pendingInvalidates = 0;
         if (b.owner==src || b.sharers.find(src)!=b.sharers.end())
         {// if new owner is already in the directory
            pendingInvalidates = b.sharers.size();
         }
         else
         {
            pendingInvalidates = b.sharers.size()+1;
         }

         // send exclusive reply as directory block response with invalidates pending
         ReadResponseMsg* reply = EM().CreateReadResponseMsg(getDeviceID());
         reply->addr = m->addr;
         reply->blockAttached = false;
         reply->directoryLookup = true;
         reply->exclusiveOwnership = true;
         reply->pendingInvalidates = pendingInvalidates;
         reply->isIntervention = false;
         reply->originalRequestingNode = m->originalRequestingNode;
         reply->satisfied = true;
         reply->size = m->size;
         reply->solicitingMessage = m->MsgID();
         
         //PerformDirectoryFetch(m,src,false,*(b.sharers.begin()));
         AutoDetermineDestSendMsg(reply,src,lookupTime+remoteSendTime,
            &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkReqExRead","OnDirBlkResp");

         // send invalidations to the sharers
         for (HashSet<NodeID>::iterator i = b.sharers.begin(); i!=b.sharers.end(); i++)
         {
            if (*i==src)
            {// skip new owner
               continue;
            }
            InvalidateMsg* invalidation = EM().CreateInvalidateMsg(getDeviceID());
            invalidation->addr = m->addr;
            invalidation->newOwner = src;
            invalidation->size = m->size;
            invalidation->solicitingMessage = m->MsgID();
            AutoDetermineDestSendMsg(invalidation,*i,lookupTime+remoteSendTime,
               &ThreeStageDirectory::OnRemoteInvalidate,"OnDirBlkReqExRead","OnRemoteInv");
         }

         // send invalidation to owner if it is not the new owner
         if (b.owner != src)
         {
            InvalidateMsg* invalidation = EM().CreateInvalidateMsg(getDeviceID());
            invalidation->addr = m->addr;
            invalidation->newOwner = src;
            invalidation->size = m->size;
            invalidation->solicitingMessage = m->MsgID();
            AutoDetermineDestSendMsg(invalidation,b.owner,lookupTime+remoteSendTime,
               &ThreeStageDirectory::OnRemoteInvalidate,"OnDirBlkReqExRead","OnRemoteInv");
         }

         b.sharers.clear();
         b.owner = src;
         // do not dispose msg here, because it is still in pendingLocalReads
         //EM().DisposeMsg(m);
         return;
      }// else if (b.owner==InvalidNodeID && b.sharers.size()!=0)
      else if (b.owner!=InvalidNodeID&&b.owner!=src)
      {// if directory state is Exclusive with another owner, transitions to Busy-exclusive with
         // requestor as owner and send out an intervention exclusive request to the previous
         // owner and a speculative reply to the requestor

         // size should be 0 if we don't allow the owner state?
         DebugAssert(b.sharers.size()==0);
         DebugAssert(pendingDirectoryBusyExclusiveReads.find(m->addr)==pendingDirectoryBusyExclusiveReads.end());

         // send intervention exclusive request to the previous owner
         NodeID previousOwner = b.owner;
         ReadMsg* read = (ReadMsg*)EM().ReplicateMsg(m);
         read->alreadyHasBlock = false;
         read->directoryLookup = false;
         read->isIntervention = true;
         read->requestingExclusive = true;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
         printDebugInfo("OnDirBlkReqShRead",*m,("pendDirShRead.insert("
            +to_string<Address>(m->addr)+")").c_str(),src);
#endif
         pendingDirectoryBusyExclusiveReads[m->addr].sourceNode = src;
         pendingDirectoryBusyExclusiveReads[m->addr].msg = read;
         pendingDirectoryBusyExclusiveReads[m->addr].previousOwner = previousOwner;
         AutoDetermineDestSendMsg(read,previousOwner,lookupTime+remoteSendTime,
            &ThreeStageDirectory::OnRemoteRead,"OnDirBlkReqExRead","OnRemRead");

         // not doing speculative replies right now
         // request speculative reply from a sharer
         /*
         ReadMsg* speculativeRequest = (ReadMsg*)EM().ReplicateMsg(m);
         speculativeRequest->alreadyHasBlock = false;
         speculativeRequest->directoryLookup = false;
         speculativeRequest->isInterventionShared = true;
         speculativeRequest->requestingExclusive = false;
         speculativeRequest->isInterventionShared = true;
         speculativeRequest->isSpeculative = true;
         AutoDetermineDestSendMsg(speculativeRequest,src,localSendTime,
            &ThreeStageDirectory::OnRemoteSpeculativeReadResponse,"OnDirBlkReqShRead","OnRemoteSpecReadRes");
            */

         b.owner = src;
         // do not dispose m here, because the original OnLocalRead still has it
         //EM().DisposeMsg(m);
         return;
      } // else if (b.owner!=InvalidNodeID&&b.owner!=src)
      else
      {
         DebugFail("Should not be here");
         return;
      }
   } // OnDirectoryBlockRequestExclusiveRead()

	void ThreeStageDirectory::OnDirectoryBlockResponse(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_ReadResponse);
      ReadResponseMsg* m = (ReadResponseMsg*)msgIn;
      DebugAssert(m->directoryLookup);
      // message must not come from memory, because we want all messages coming from memory to return to directory first
      //2010/09/24 Eric
      // don't do this because we are changing the message coming from memory to go to directory first
      //DebugAssert(src != memoryNodeCalc->CalcNodeID(m->addr));
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
		printPendingDirectorySharedReads();
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printPendingLocalReads();
#endif

      if (m->evictionMessage)
      {
         // eviction messages don't have to be satisfied if OnLocalReadResponse
            // managed to return before OnLocalEviction
         //DebugAssert(m->satisfied);
         DebugAssert(m->pendingInvalidates == 0);
         DebugAssert(!m->hasPendingMemAccesses);

         // evictionResponse could have arrived after interventionResponse
         if (pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end() && m->satisfied)
         {
            SendLocalReadResponse(m);
         }
         else
         {// if the response coming from eviction has already been satisfied or
            // m is not a satisfied message
            EM().DisposeMsg(m);
         }
      }
      else
      {
         if (m->isIntervention)
         {
            // send intervention complete message to directory so it knows to unblock directory
            DebugAssert(m->pendingInvalidates==0);
            DebugAssert(m->satisfied);
            InterventionCompleteMsg *toDir = EM().CreateInterventionCompleteMsg(getDeviceID());
            toDir->addr = m->addr;
            toDir->solicitingMessage = m->solicitingMessage;

            // use 0 time to simulate instantaneous sending
            AutoDetermineDestSendMsg(toDir,directoryNodeCalc->CalcNodeID(m->addr),remoteSendTime,
               &ThreeStageDirectory::OnRemoteInterventionComplete,"OnDirBlkResp","OnRemIntCom");
         }

		   // check that m->solicitingMessage is in pendingLocalReads before accessing it.
		   // Error here probably means that the directoryBlockResponse was sent twice or 
         // was sent to the wrong node.
         DebugAssert(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end());
		   const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];

		   if(!m->satisfied)
		   {
   #ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		      printDebugInfo("OnDirBlkResp",*m,
		            ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
   #endif
			   pendingLocalReads.erase(m->solicitingMessage);
   #ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
            printDebugInfo("OnLocalRead",*m,"OnDirBlkResp");
   #endif
            ReversePendingLocalReadData &myData = reversePendingLocalReads[m->addr];

            if (ref->requestingExclusive && myData.isExclusiveReadSatisfiedByEviction)
            {// eraseReversePendingLocalRead depends on myData.isExclusiveReadSatisfiedByEviction
               myData.isExclusiveReadSatisfiedByEviction = false;
               EraseReversePendingLocalRead(m,ref);
               EM().DisposeMsg(ref);
               EM().DisposeMsg(m);
            }
            else if (myData.isSharedReadSatisfiedByEviction)
            {// eraseReversePendingLocalRead depends on myData.isSharedReadSatisfiedByEviction
               myData.isSharedReadSatisfiedByEviction = false;
               EraseReversePendingLocalRead(m,ref);
               EM().DisposeMsg(ref);
               EM().DisposeMsg(m);
            }
            else
            {
               EraseReversePendingLocalRead(m,ref);
			      OnLocalRead(ref);
            }
			   return;
         }
         // m is satisfied

         // check if we have to wait for invalidates
         if (m->pendingInvalidates > 0)
         {// there are pending invalidates, so put this reply into a queue

            // invalidate responses could arrive before DirBlkResponse
            //DebugAssert(waitingForInvalidates.find(m->addr)==waitingForInvalidates.end());
            DebugAssert(!m->isWaitingForInvalidateUnlock);
            InvalidateData &id = waitingForInvalidates[m->addr];
            DebugAssert(id.msg==NULL);
            id.msg = m;
            id.invalidatesWaitingFor = m->pendingInvalidates;
            if (id.invalidatesReceived==id.invalidatesWaitingFor)
            {
               HandleReceivedAllInvalidates(m->addr);
            }
            return;
         }
         else
         {// else, pendingInvalidates == 0
            //send local read response to cache above
            DebugAssert(!m->pendingInvalidates);
            DebugAssert(waitingForInvalidates.find(m->addr)==waitingForInvalidates.end());
            SendLocalReadResponse(m);
         }

         if (m->hasPendingMemAccesses)
         {// schedule memAccessComplete to be sent after we give enough time
            // for readResponse to go through
				CBSendMemAccessComplete::FunctionType* f = cbSendMemAccessComplete.Create();
            // src should be the directory
            f->Initialize(this,m->addr,src);
				EM().ScheduleEvent(f,satisfyTime + localSendTime);
         }

         if (m->isWaitingForInvalidateUnlock)
         {
            ReadCompleteMsg *rcm = EM().CreateReadCompleteMsg(getDeviceID());
            rcm->addr = m->addr;
            rcm->solicitingMessage = m->solicitingMessage;
            NodeID directoryNode = directoryNodeCalc->CalcNodeID(m->addr);
            AutoDetermineDestSendMsg(rcm, directoryNode, remoteSendTime,
               &ThreeStageDirectory::OnRemoteReadComplete,"OnDirBlkResp","OnRemReadCom");
         }
		} // else m is not from eviction
	} //ThreeStageDirectory::OnDirectoryBlockResponse

	void ThreeStageDirectory::Initialize(EventManager* em, const RootConfigNode& config, const std::vector<Connection*>& connectionSet)
	{
		BaseMemDevice::Initialize(em,config,connectionSet);
		localConnectionID = remoteConnectionID = -1;
		for(int i = 0; i < ConnectionCount(); i++)
		{
			if(GetConnection(i).Name() == "LocalConnection")
			{
				localConnectionID = i;
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
		DebugAssert(localConnectionID != -1 && remoteConnectionID != -1);
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
		const RootConfigNode& memCalc = Config::GetSubRoot(config,"MemoryNodeCalculator");
		if(Config::GetString(dirCalc, "Type") == "HashedPageCalculator")
		{
			directoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown directory node calculator type");
		}
		if(Config::GetString(memCalc, "Type") == "HashedPageCalculator")
		{
			memoryNodeCalc = new HashedPageCalculator();
		}
		else
		{
			DebugFail("Unknown memory node calculator type");
		}
		directoryNodeCalc->Initialize(dirCalc);
		memoryNodeCalc->Initialize(memCalc);

		messagesReceived = 0;
	}
	/**
	 * this is used for checkpoint purposes
	 */
	void ThreeStageDirectory::DumpRunningState(RootConfigNode& node)
	{}
	/**
	 * put anything here that you might want to output to the report file
	 */
	void ThreeStageDirectory::DumpStats(std::ostream& out)
	{
	   out << "messagesReceived:" << messagesReceived << std::endl;
	}
	/**
	 * Handles all the incoming messages from outside of the directory.
	 * The message can come from the cache side or from the network.
	 */
	void ThreeStageDirectory::RecvMsg(const BaseMsg* msg, int connectionID)
	{
	   messagesReceived++;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_COUNTERS
	   cout << threeStageDirectoryEraseDirectoryShareCounter << endl;
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_MSG_COUNT
	   cout << "ThreeStageDirectory::RecvMsg: " << memoryDirectoryGlobalInt++ << ' ' << endl;
#endif
		DebugAssert(msg);

		if(connectionID == localConnectionID)
		{
			switch(msg->Type())
			{
			case(mt_Read):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
			      printDebugInfo("OnLocalRead",*msg,"RecvMsg");
#endif
					OnLocalRead(msg);
					break;
				}
			case(mt_ReadResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnLocReadRes",*msg,"RecvMsg");
#endif
					OnLocalReadResponse(msg);
					break;
				}
			case(mt_Write):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnLocalWrite",*msg,"RecvMsg");
#endif
					OnLocalWrite(msg);
					break;
				}
			case(mt_Eviction):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnLocalEviction",*msg,"RecvMsg");
#endif
					OnLocalEviction(msg);
					break;
				}
			case(mt_InvalidateResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnLocalInvalidateResponse",*msg,"RecvMsg");
#endif
					OnLocalInvalidateResponse(msg);
					break;
				}
			default:
				DebugFail("Msg Type not handled");
			}
		}
		else if(connectionID == remoteConnectionID)
		{
			DebugAssert(msg->Type() == mt_Network);
			const NetworkMsg* m = (const NetworkMsg*)msg;
			const BaseMsg* payload = m->payloadMsg;
			NodeID src = m->sourceNode;
			DebugAssert(m->destinationNode == nodeID);
			EM().DisposeMsg(m);
			switch(payload->Type())
			{
			case(mt_Read):
				{
					ReadMsg* m = (ReadMsg*)payload;
					if(m->directoryLookup)
					{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnDirBlkRequ",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockRequest(m,src);
					}
					else
					{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemRead",*m,"RecvMsg",src);
#endif
						OnRemoteRead(m,src);
					}
					break;
				}
			case(mt_ReadResponse):
				{
					ReadResponseMsg* m = (ReadResponseMsg*)payload;
					if(m->directoryLookup)
					{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnDirBlkResp",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockResponse(m,src);
					}
               // not doing speculative replies right now
               /*
               else if (m->isSpeculative)
               {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteSpeculativeReadResponse",*m,"RecvMsg",src);
#endif
			      OnRemoteSpeculativeReadResponse(m,src);
               }
               */
					else
					{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemReadRes",*m,"RecvMsg",src);
#endif
						OnRemoteReadResponse(m,src);
					}
					break;
				}
			case(mt_WriteResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteWriteRes",*payload,"RecvMsg",src);
#endif
					OnRemoteWriteResponse(payload,src);
					break;
				}
			case(mt_Eviction):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemEvic",*payload,"RecvMsg",src);
#endif
					OnRemoteEviction(payload,src);
					break;
				}
			case(mt_EvictionResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemEvicResponse",*payload,"RecvMsg",src);
#endif
					OnRemoteEvictionResponse(payload,src);
					break;
				}
			case(mt_EvictionBusyAck):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemEvicBusyAck",*payload,"RecvMsg",src);
#endif
					OnRemoteEvictionBusyAck(payload,src);
					break;
				}
			case(mt_Invalidate):
            {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteInvalidate",*payload,"RecvMsg",src);
#endif
					OnRemoteInvalidate(payload,src);
					break;
				}
			case(mt_InvalidateResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteInvalidateResponse",*payload,"RecvMsg",src);
#endif
					OnRemoteInvalidateResponse(payload,src);
					break;
				}
			case(mt_MemAccessComplete):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteMemAccessComplete",*payload,"RecvMsg",src);
#endif
					OnRemoteMemAccessComplete(payload,src);
					break;
				}
			case(mt_InterventionComplete):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteInterventionComplete",*payload,"RecvMsg",src);
#endif
					OnRemoteInterventionComplete(payload,src);
					break;
				}
			case(mt_InvalidationComplete):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemInvCom",*payload,"RecvMsg",src);
#endif
					OnRemoteInvalidationComplete(payload,src);
					break;
				}
			case(mt_ReadComplete):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemReadCom",*payload,"RecvMsg",src);
#endif
					OnRemoteReadComplete(payload,src);
					break;
				}
			case(mt_UnrequestedReadResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemUnreqReadRes",*payload,"RecvMsg",src);
#endif
					OnRemoteUnrequestedReadResponse(payload,src);
					break;
				}
			default:
				DebugFail("Payload type unrecognized");
			}
		}
		else
		{
			DebugFail("Connection not a valid ID");
		}
	}

   void ThreeStageDirectory::printDirectoryData(Address myAddress, MessageID myMessageID)
   {
      DebugAssert(directoryData.find(myAddress)!=directoryData.end());

      bool isSharedBusy = (pendingDirectoryBusySharedReads.find(myAddress)!=pendingDirectoryBusySharedReads.end());
      bool isExclusiveBusy = (pendingDirectoryBusyExclusiveReads.find(myAddress)!=pendingDirectoryBusyExclusiveReads.end());
      bool hasPendingMemAccess = (pendingMainMemAccesses.find(myAddress)!=pendingMainMemAccesses.end());
      bool isWaitingForInvalidationComplete = (waitingForInvalidationComplete.find(myAddress)!=waitingForInvalidationComplete.end());
      bool isWaitingForReadResponse = (waitingForReadResponse.find(myAddress)!=waitingForReadResponse.end());
      
      directoryData[myAddress].print(myAddress, myMessageID,isSharedBusy, isExclusiveBusy, hasPendingMemAccess,
         isWaitingForInvalidationComplete,isWaitingForReadResponse);
   }

	/**
	 * readMsgArray in this method could be coupled with Eclipse's debugger to
	 * see what elements are in pendingDirectoryBusySharedReads. The reason for using
	 * a regular array instead of a vector is because Eclipse can view
	 * Array elements directly, but not elements in STL containers.
	 */
	void ThreeStageDirectory::printPendingDirectoryBusySharedReads()
	{
	   HashMap<Address, LookupData<ReadMsg> >::const_iterator myIterator;


	   myIterator = pendingDirectoryBusySharedReads.begin();
      cout << "ThreeStageDirectory::printPendingDirectorySharedReads: ";

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
	void ThreeStageDirectory::printPendingLocalReads()
	{
      HashMap<MessageID, const ReadMsg*>::const_iterator myIterator;

      myIterator = pendingLocalReads.begin();

      cout << "ThreeStageDirectory::printPendingLocalReads: ";

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

   void ThreeStageDirectory::printDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation)
   {
      printBaseMemDeviceDebugInfo("3SDir", fromMethod, myMessage, operation);
   }

   void ThreeStageDirectory::printDebugInfo(const char* fromMethod, const BaseMsg &myMessage,
         const char* operation, NodeID src)
   {
      printBaseMemDeviceDebugInfo("3SDir", fromMethod, myMessage, operation, src);
   }

   void ThreeStageDirectory::printDebugInfo(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(17) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(3) << getDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " 3SDir:" << fromMethod
            << " " << operation << "(" << Memory::convertNodeIDToDeviceID(id) << ")"
            << endl;
   }

   void ThreeStageDirectory::printEraseOwner(const char* fromMethod,Address addr,NodeID id,const char* operation)
   {
      cout << setw(17) << " " // account for spacing from src and msgSrc
            << " dst=" << setw(2) << getDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " 3SDir:" << fromMethod
            << " nodeID=" << id
            << " " << operation
            << endl;
   }
}
