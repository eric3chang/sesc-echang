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
//#define MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
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
/*
	void ThreeStageDirectory::PerformDirectoryFetch(Address a, NodeID src)
	{
      // commented out because of 3-stage
      // TODO: 2010/09/02 3-stage modification
      //m->directoryLookup = false;
	   ReadMsg* m = EM().CreateReadMsg(getDeviceID());
      //EM().DisposeMsg(msgIn);
      m->onCompletedCallback = NULL;
      m->alreadyHasBlock = false;
      m->originalRequestingNode = src;
      m->addr = a;
      m->onCompletedCallback = NULL;
      m->size = 64;
      m->requestingExclusive = false;
      BlockData& b = directoryData[a];
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
         target = memoryNodeCalc->CalcNodeID(a);
      }
      if(target == nodeID)
      {
   #ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteRead",*m,"PerformDirectoryFetch(addr,node)",nodeID);
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
*/
   // performs a directory fetch
	void ThreeStageDirectory::PerformDirectoryFetch(const ReadMsg* msgIn, NodeID src)
	{
	   // commented out because of 3-stage
		// TODO: 2010/09/02 3-stage modification
		//m->directoryLookup = false;
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
               printDebugInfo("OnRemoteRead",*m,"PerformDirectoryFetch(ReadMsg,src)",nodeID);
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

   // performs a directory fetch
	void ThreeStageDirectory::PerformDirectoryFetch(const ReadMsg* msgIn,NodeID src,bool isExclusive,NodeID target)
	{
      DebugAssert(src != InvalidNodeID);
      DebugAssert(target != InvalidNodeID);
	   ReadMsg* m = (ReadMsg*)EM().ReplicateMsg(msgIn);
		m->alreadyHasBlock = false;
      m->originalRequestingNode = src;
      m->requestingExclusive = isExclusive;
      NodeID memoryNode = memoryNodeCalc->CalcNodeID(m->addr);
		// directoryLookup is true only for memory because we want to return a
		   // OnDirectoryBlockResponse from memory
		if(target==memoryNode)
		{
		   m->directoryLookup = true;
		}
		else
		{
		   m->directoryLookup = false;
		}
		if(target == nodeID)
		{
         DebugAssert(m->directoryLookup==false);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteRead",*m,"PerformDirectoryFetch(msgIn,src,isExclusive,target)",nodeID);
#endif
			OnRemoteRead(m, nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->destinationNode = target;
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
			{
			   nm->sourceNode = nodeID;
			}
			nm->payloadMsg = m;
			SendMsg(remoteConnectionID, nm, lookupTime + remoteSendTime);
		}
      // do not dispose message here, because the original OnLocalRead might still need it
      //EM().DisposeMsg(msgIn);
	}

	/*
   void ThreeStageDirectory::PerformDirectoryFetchOwner(const ReadMsg* msgIn, NodeID src)
   {
      // commented out because of 3-stage
      // TODO: 2010/09/02 3-stage modification
      //m->directoryLookup = false;
      ReadMsg* m = (ReadMsg*)EM().ReplicateMsg(msgIn);
      EM().DisposeMsg(msgIn);
      m->onCompletedCallback = NULL;
      m->alreadyHasBlock = false;
      m->originalRequestingNode = src;
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
      // TODO added this because we want to fetch from memory only for owner
      DebugAssert(target == memoryNodeCalc->CalcNodeID(m->addr));
      if(target == nodeID)
      {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteRead",*m,"PerformDirectoryFetchOwner(ReadMsg,node)",nodeID);
#endif
         //TODO 2010/09/02 Eric, change this because of 3-stage directory
         OnRemoteRead(m, nodeID);//ERROR
         // on remoteRead should know to use m->requestingNode
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
   }  // performDirectoryFetchOwner
*/

   /**
   send local read response to the cache above
   */
   void ThreeStageDirectory::SendLocalReadResponse(const ReadResponseMsg* m)
   {
      DebugAssert(pendingLocalReads.find(m->solicitingMessage)!=pendingLocalReads.end());
      const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];
      
		ReadResponseMsg* r = EM().CreateReadResponseMsg(getDeviceID(),ref->GeneratingPC());
		r->addr = ref->addr;
		r->blockAttached = m->blockAttached;
		r->directoryLookup = false;
		r->exclusiveOwnership = m->exclusiveOwnership;
		r->satisfied = true;
		r->size = ref->size;
		r->solicitingMessage = ref->MsgID();
		EM().DisposeMsg(ref);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printDebugInfo("OnDirectoryBlockResponse", *m,
		   ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
#endif
		pendingLocalReads.erase(m->solicitingMessage);

		EM().DisposeMsg(m);
		SendMsg(localConnectionID, r, satisfyTime + localSendTime);
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

	/**
	 * the message came from the local (cpu) side. It is a read msg
	 */
	void ThreeStageDirectory::OnLocalRead(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
#if defined DEBUG && defined _WIN32
      MessageID tempMessageID = m->MsgID();
#endif
		NodeID remoteNode = directoryNodeCalc->CalcNodeID(m->addr);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printDebugInfo("OnLocalRead", *m,
		      ("pendingLocalReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
		DebugAssert(pendingLocalReads.find(m->MsgID()) == pendingLocalReads.end());
		pendingLocalReads[m->MsgID()] = m;
		ReadMsg* forward = (ReadMsg*)EM().ReplicateMsg(m);
		forward->onCompletedCallback = NULL;
		forward->directoryLookup = true;
		forward->originalRequestingNode = nodeID;
		if(remoteNode == nodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
		   printDebugInfo("OnDirectoryBlockRequest",*m,"OnLocalRead");
#endif
			CBOnDirectoryBlockRequest::FunctionType* f = cbOnDirectoryBlockRequest.Create();
			f->Initialize(this,forward,nodeID);
			EM().ScheduleEvent(f, localSendTime);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			DebugAssert(nm);
			nm->destinationNode = remoteNode;
			nm->sourceNode = nodeID;
			nm->payloadMsg = forward;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
	}
	void ThreeStageDirectory::OnLocalReadResponse(const BaseMsg* msgIn)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_ReadResponse);
      ReadResponseMsg* m = (ReadResponseMsg*)msgIn;
#if defined DEBUG && defined _WIN32
      MessageID tempMsgID = m->MsgID();
#endif
		DebugAssert(pendingRemoteReads.find(m->solicitingMessage)!=pendingRemoteReads.end());
      LookupData<ReadMsg> &d = pendingRemoteReads[m->solicitingMessage];
      DebugAssert(d.msg->originalRequestingNode != InvalidNodeID);
      DebugAssert(m->satisfied);

		if (d.msg->isIntervention)
		{
         DebugAssert(m->blockAttached);
         // send response back to the requester
         ReadResponseMsg* forward = (ReadResponseMsg*)EM().ReplicateMsg(m);
         forward->originalRequestingNode = d.msg->originalRequestingNode;
         // changed this to true because of 3-stage directory
         forward->directoryLookup = true;
         forward->isIntervention = true;
         //forward->isSpeculative = d.msg->isSpeculative;
         // the destination node is the original requesting node, and not the directory
         AutoDetermineDestSendMsg(forward,d.msg->originalRequestingNode,remoteSendTime+lookupTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnLocalReadResponse","OnDirBlkResponse");

         // send response back to the directory
         ReadResponseMsg* dirResponse = (ReadResponseMsg*)EM().ReplicateMsg(m);
         DebugAssert(d.msg->originalRequestingNode != InvalidNodeID);
         dirResponse->originalRequestingNode = d.msg->originalRequestingNode;
         dirResponse->directoryLookup = false;
         dirResponse->isIntervention = true;
         //forward->isSpeculative = d.msg->isSpeculative;
         AutoDetermineDestSendMsg(dirResponse,d.sourceNode,remoteSendTime+lookupTime,
               &ThreeStageDirectory::OnRemoteReadResponse,"OnLocalReadResponse","OnRemoteReadResponse");
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
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnLocalReadResponse","OnDirBlkResponse");
		}
      */
      else
      {
         // send response back to the requester
         ReadResponseMsg* forward = (ReadResponseMsg*)EM().ReplicateMsg(m);
         forward->originalRequestingNode = d.msg->originalRequestingNode;
         // changed this to true because of 3-stage directory
         forward->directoryLookup = true;
         forward->isIntervention = false;
         //forward->isSpeculative = d.msg->isSpeculative;
         // the destination node is the original requesting node, and not the directory
         AutoDetermineDestSendMsg(forward,d.msg->originalRequestingNode,remoteSendTime+lookupTime,
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnLocalReadResponse","OnDirBlkResponse");
      }
      // cannot dispose this message because it's in OnLocalRead
      //EM().DisposeMsg(pendingRemoteReads[m->solicitingMessage].msg);
      EM().DisposeMsg(m);
      pendingRemoteReads.erase(m->solicitingMessage);
      return;

		/*(
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_READS
      printDebugInfo("OnLocalReadResponse",*m,
            ("pendingRemoteReads.erase("+to_string<MessageID>(d.msg->MsgID())+")").c_str());
#endif
      pendingRemoteReads.erase(d.msg->MsgID());
      EM().DisposeMsg(d.msg);
      // d.msg might be the same as m
      EM().DisposeMsg(m);
*/
		/*
		 // InvalidateSharerMsg related code
      if (m->satisfied)
      {
         if (unsatisfiedRequests.find(m->solicitingMessage) != unsatisfiedRequests.end())
         {
            unsatisfiedRequests.erase(m->solicitingMessage);
         }
      }
      else // if unsatisfied for too many times, send invalidateSharer back to directory
		{
         unsatisfiedRequests[m->solicitingMessage]++;
         if (unsatisfiedRequests[m->solicitingMessage] >= 15)
         {
            InvalidateSharerMsg *forward = EM().CreateInvalidateSharerMsg(getDeviceID());
            forward->addr = d.msg->addr;
            NodeID dirNodeID = directoryNodeCalc->CalcNodeID(d.msg->addr);

		      //TODO 2010/09/09 Eric
            if (dirNodeID==nodeID)
            {
   #ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnInvalidateSharerMsg",*forward,"OnLocalReadResponse",nodeID);
   #endif
               OnRemoteInvalidateSharer(forward,nodeID);
               return;
            }
            else
            {
               NetworkMsg* nmToDir = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
               nmToDir->payloadMsg = forward;
               nmToDir->sourceNode = nodeID;
               nmToDir->destinationNode = dirNodeID;
               SendMsg(remoteConnectionID, nmToDir, remoteSendTime);
            }
         }// if (unsatisfiedRequests[m->solicitingMessage] >= 3)
		} // else (!m->satisfied)
		*/
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
		DebugAssert(pendingEviction.find(m->addr) == pendingEviction.end())
		pendingEviction.insert(m->addr);
		EvictionResponseMsg* erm = EM().CreateEvictionResponseMsg(getDeviceID(),m->GeneratingPC());
		erm->addr = m->addr;
		erm->size = m->size;
		erm->solicitingMessage = m->MsgID();
		SendMsg(localConnectionID, erm, localSendTime);
		NodeID id = directoryNodeCalc->CalcNodeID(m->addr);
		if(id == nodeID)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteEviction",*m,"OnLocalEviction",nodeID);
#endif
			OnRemoteEviction(m,nodeID);
		}
		else
		{
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
			nm->destinationNode = id;
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
		DebugAssert(pendingRemoteInvalidates.find(m->addr) != pendingRemoteInvalidates.end());
		LookupData<InvalidateMsg>& d = pendingRemoteInvalidates[m->addr];
		DebugAssert(d.msg->MsgID() == m->solicitingMessage);
      //TODO 2010/09/13 Eric
      //if(nodeID != d.sourceNode)
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
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_INVALIDATES
		printDebugInfo("OnLocalInvalidateResponse",*m,
		   ("PRI.erase("+to_string<Address>(d.msg->addr)+")").c_str());
#endif
		pendingRemoteInvalidates.erase(d.msg->addr);
	}

	void ThreeStageDirectory::OnRemoteRead(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
		DebugAssert(!m->directoryLookup);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_REMOTE_READS
		printDebugInfo("OnRemoteRead", *m,
		      ("pendingRemoteReads.insert("+to_string<MessageID>(m->MsgID())+")").c_str());
#endif
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
		DebugAssert(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end()
         || pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end());
		DebugAssert(pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end()
         || pendingDirectoryExclusiveReads.find(m->addr) == pendingDirectoryExclusiveReads.end());
		DebugAssert(directoryData.find(m->addr) != directoryData.end());
		/*
		if(m->satisfied)
		{
			if(m->exclusiveOwnership)
			{
				EraseDirectoryShare(m->addr,src);
			}
			// if there is some pending shared read on this address
			if(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end())
			{
			   // for all the elements in pendingDirectorySharedReads where key is in the range of m->addr
				for(HashMap<Address,LookupData<ReadMsg> >::iterator i = pendingDirectorySharedReads.equal_range(m->addr).first; i != pendingDirectorySharedReads.equal_range(m->addr).second; i++)
				{
					ReadResponseMsg* r = EM().CreateReadResponseMsg(getDeviceID(),i->second.msg->GeneratingPC());
					r->blockAttached = true;
					r->addr = m->addr;
					r->size = m->size;
					r->directoryLookup = true;
					r->exclusiveOwnership = (pendingDirectorySharedReads.equal_range(m->addr).first++) == pendingDirectorySharedReads.equal_range(m->addr).second;// only one reader
					r->satisfied = true;
					r->solicitingMessage = i->second.msg->MsgID();
					AddDirectoryShare(m->addr,i->second.sourceNode,false);
					if(i->second.sourceNode == nodeID)
					{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnDirectoryBlockResponse",*r,"OnRemoteReadResponse",nodeID);
#endif
						OnDirectoryBlockResponse(r,nodeID);
					}
					else
					{
						NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(),i->second.msg->GeneratingPC());
						nm->destinationNode = i->second.sourceNode;
						nm->sourceNode = nodeID;
						nm->payloadMsg = r;
						SendMsg(remoteConnectionID, nm, remoteSendTime + satisfyTime);
					}
					EM().DisposeMsg(i->second.msg);
				}
				pendingDirectorySharedReads.erase(pendingDirectorySharedReads.equal_range(m->addr).first,pendingDirectorySharedReads.equal_range(m->addr).second);
			}  //if(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end())
			else // pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end())
			{  // there is no pending shared read on this address
				DebugAssert(m->exclusiveOwnership);
				DebugAssert(m->blockAttached);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
				printDebugInfo("OnRemoteReadResponse", *m,
				      ("directoryData[m->addr].owner="+to_string<NodeID>(
				            directoryData[m->addr].owner)).c_str(), src);
#endif
				DebugAssert(directoryData[m->addr].owner == InvalidNodeID || directoryData[m->addr].owner == src);
				if(directoryData[m->addr].sharers.size() == 0)
				{//send block on now
					ReadResponseMsg* response = (ReadResponseMsg*)EM().ReplicateMsg(m);
					response->directoryLookup = true;
					if(pendingDirectoryExclusiveReads[m->addr].sourceNode != nodeID)
					{
						NetworkMsg* net = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
						net->sourceNode = nodeID;
						net->destinationNode = pendingDirectoryExclusiveReads[m->addr].sourceNode;
						net->payloadMsg = response;
						SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
					}
					else
					{
						SendMsg(localConnectionID, response, lookupTime + localSendTime);
					}
					AddDirectoryShare(m->addr,pendingDirectoryExclusiveReads[m->addr].sourceNode,true);
					EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
					pendingDirectoryExclusiveReads.erase(m->addr);
				} // if(directoryData[m->addr].sharers.size() == 0)
				else
				{//hold the block, send on once all invalidations are complete
					for(HashSet<NodeID>::iterator i = directoryData[m->addr].sharers.begin(); i != directoryData[m->addr].sharers.end(); i++)
					{
						InvalidateMsg* inv = EM().CreateInvalidateMsg(getDeviceID(),m->GeneratingPC());
						inv->addr = m->addr;
						inv->size = m->size;
						if(*i != nodeID)
						{
							NetworkMsg* net = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
							net->destinationNode = *i;
							net->sourceNode = nodeID;
							net->payloadMsg = inv;
							SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
						}
						else
						{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteInvalidate",*inv,"OnRemoteReadResponse",nodeID);
#endif
							OnRemoteInvalidate(inv, nodeID);
						}
					}
				} // else (directoryData[m->addr].sharers.size() != 0)
			} // else pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end())
			EM().DisposeMsg(m);
		}// if(m->satisfied)
	   */
      DebugAssert(m->originalRequestingNode!=InvalidNodeID);
      // directory receives shared writeback/transfer, updates memory if it is shared writeback,
         // then transitions to the shared state

      // if we should ignore this intervention
      if (pendingIgnoreInterventions.find(m->addr)!=pendingIgnoreInterventions.end())
      {
         pendingIgnoreInterventions.erase(m->addr);
         if (pendingDirectorySharedReads.find(m->addr)!=pendingDirectorySharedReads.end())
         {
            EM().DisposeMsg(pendingDirectorySharedReads[m->addr].msg);
            pendingDirectorySharedReads.erase(m->addr);
         }
         else if (pendingDirectoryExclusiveReads.find(m->addr)!=pendingDirectoryExclusiveReads.end())
         {
            EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
            pendingDirectoryExclusiveReads.erase(m->addr);
         }
         EM().DisposeMsg(m);
         return;
      }

      DebugAssert(m->isIntervention);
		DebugAssert(m->satisfied);
      if (m->isIntervention && pendingDirectorySharedReads.find(m->addr)!=pendingDirectorySharedReads.end())
      {
         if(m->blockAttached)
         {
            WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), m->GeneratingPC());
            wm->addr = m->addr;
            wm->size = m->size;
            wm->onCompletedCallback = NULL;
            NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
            nm->sourceNode = nodeID;
            nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
            nm->payloadMsg = wm;
            SendMsg(remoteConnectionID, nm, remoteSendTime);
         }
         LookupData<ReadMsg> &ld = pendingDirectorySharedReads[m->addr];
         ChangeOwnerToShare(m->addr,ld.sourceNode);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
         printDebugInfo("OnRemoteReadResponse",*m,("pendDirShRead.erase("
            +to_string<Address>(m->addr)+")").c_str(),src);
#endif
         EM().DisposeMsg(pendingDirectorySharedReads[m->addr].msg);
         pendingDirectorySharedReads.erase(m->addr);
      }
      else if (m->isIntervention && pendingDirectoryExclusiveReads.find(m->addr)!=pendingDirectoryExclusiveReads.end())
      {
         // block should be attached no matter what because we requested from previous owner
         DebugAssert(m->blockAttached);
         if(m->blockAttached)
         {
            WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), m->GeneratingPC());
            wm->addr = m->addr;
            wm->size = m->size;
            wm->onCompletedCallback = NULL;
            NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
            nm->sourceNode = nodeID;
            nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
            nm->payloadMsg = wm;
            SendMsg(remoteConnectionID, nm, remoteSendTime);
         }
         LookupData<ReadMsg> &ld = pendingDirectoryExclusiveReads[m->addr];
         BlockData &b = directoryData[m->addr];
         // transition to exclusive state with new owner
         b.sharers.clear();
         b.owner = ld.sourceNode;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
         printDebugInfo("OnRemoteReadResponse",*m,("pendDirExRead.erase("
            +to_string<Address>(m->addr)+")").c_str(),src);
#endif
         EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
         pendingDirectoryExclusiveReads.erase(m->addr);
      }
      else
      {
         DebugFail("should not be here");
      }
      
      EM().DisposeMsg(m);
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
		EM().DisposeMsg(m);
	}
	void ThreeStageDirectory::OnRemoteEviction(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Eviction);
      EvictionMsg* m = (EvictionMsg*)msgIn;
		DebugAssert(directoryData.find(m->addr) != directoryData.end());
		BlockData& b = directoryData[m->addr];

      // if directory state is Exclusive with requestor as owner,
         // transitions to Unowned and returns a writeback exclusive acklowledge
         // to the requestor
      if (b.owner==src && b.sharers.size()==0)
      {
         DebugAssert(pendingDirectorySharedReads.find(m->addr)==pendingDirectorySharedReads.end());
         DebugAssert(pendingDirectoryExclusiveReads.find(m->addr)==pendingDirectoryExclusiveReads.end());
         EvictionResponseMsg* rm = EM().CreateEvictionResponseMsg(getDeviceID(),m->GeneratingPC());
			rm->addr = m->addr;
         rm->isExclusive = true;
			rm->size = m->size;
			rm->solicitingMessage = m->MsgID();

         AutoDetermineDestSendMsg(rm,src,remoteSendTime,&ThreeStageDirectory::OnRemoteEvictionResponse
            ,"OnRemoteEviction","OnRemoteEvictionResponse");

         b.owner=InvalidNodeID;
      }
      else if (pendingDirectorySharedReads.find(m->addr)!=pendingDirectorySharedReads.end())
      {// if directory state is Busy-shared, transitions to shared, a shared response is returned to
         // the owner marked in the directory. A writeback busy acknowledgement is also sent to the requestor
         DebugAssert(pendingDirectoryExclusiveReads.find(m->addr)==pendingDirectoryExclusiveReads.end());

         // send shared response to the owner marked in the directory
         DebugAssert(b.owner!=InvalidNodeID);
         EvictionResponseMsg *sharedResponse = EM().CreateEvictionResponseMsg(getDeviceID());
         sharedResponse->addr = m->addr;
         sharedResponse->isExclusive = false;
         sharedResponse->size = m->size;
         sharedResponse->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(sharedResponse,b.owner,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionResponse,"OnRemoteEviction","OnRemoteEvictionRes");

         // send writeback busy acknowledgement to the requestor
         EvictionBusyAckMsg *busyAck = EM().CreateEvictionBusyAckMsg(getDeviceID());
         busyAck->addr = m->addr;
         busyAck->isExclusive = false;
         busyAck->size = m->size;
         busyAck->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(busyAck,src,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionBusyAck,"OnRemoteEviction","OnRemoteEvictionBusyAck");

         // transitions from busy-shared to shared
         EM().DisposeMsg(pendingDirectorySharedReads[m->addr].msg);
         pendingDirectorySharedReads.erase(m->addr);
      }
      else if (pendingDirectoryExclusiveReads.find(m->addr)!=pendingDirectoryExclusiveReads.end())
      {
         // if directory state is Busy-exclusive, transitions to Exclusive,
            // an exclusive response is returned to the owner marked in the directory
         // A writeback busy acknowledgement is also sent to the requestor
         DebugAssert(pendingDirectorySharedReads.find(m->addr)==pendingDirectorySharedReads.end());
         DebugAssert(b.sharers.size()==0);

         // send exclusive response back to owner marked in the directory
         DebugAssert(b.owner!=InvalidNodeID);
         EvictionResponseMsg *response = EM().CreateEvictionResponseMsg(getDeviceID());
         response->addr = m->addr;
         response->isExclusive = true;
         response->size = m->size;
         response->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(response,b.owner,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionResponse,"OnRemoteEviction","OnRemoteEvictionRes");

         // send writeback busy acknowledgement to requestor
         EvictionBusyAckMsg *busyAck = EM().CreateEvictionBusyAckMsg(getDeviceID());
         busyAck->addr = m->addr;
         busyAck->isExclusive = true;
         busyAck->size = m->size;
         busyAck->solicitingMessage = m->MsgID();
         AutoDetermineDestSendMsg(busyAck,src,remoteSendTime,
            &ThreeStageDirectory::OnRemoteEvictionBusyAck,"OnRemoteEviction","OnRemoteEvictionBusyAck");

         DebugFail("Unimplemented");

         // transitions from busy-exclusive to exclusive
         EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
         pendingDirectoryExclusiveReads.erase(m->addr);
      }
      else
      {
         DebugFail("should not reach here");
      }

      // if block is attached, write back to memory
		if(m->blockAttached)
		{
			WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), m->GeneratingPC());
			wm->addr = m->addr;
			wm->size = m->size;
			wm->onCompletedCallback = NULL;
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
			nm->payloadMsg = wm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
      EM().DisposeMsg(m);
	}

	void ThreeStageDirectory::OnRemoteEvictionResponse(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_EvictionResponse);
      EvictionResponseMsg* m = (EvictionResponseMsg*)msgIn;
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_EVICTION
      printDebugInfo("OnRemoteEvictionResponse", *m,
            ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str());
#endif
		DebugAssert(pendingEviction.find(m->addr) != pendingEviction.end());
		pendingEviction.erase(m->addr);
		SendMsg(localConnectionID, m, localSendTime);
	}

   void ThreeStageDirectory::OnRemoteEvictionBusyAck(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_EvictionBusyAck);
      EvictionBusyAckMsg* m = (EvictionBusyAckMsg*)msgIn;

      DebugAssert(pendingDirectorySharedReads.find(m->addr) != pendingDirectorySharedReads.end()
         || pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end());
      DebugAssert(pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end()
         || pendingDirectoryExclusiveReads.find(m->addr) == pendingDirectoryExclusiveReads.end());
      pendingIgnoreInterventions.insert(m->addr);
      /*
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_EVICTION
      printDebugInfo("OnRemoteEvictionBusyAck", *m,
            ("pendingEviction.erase("+to_string<Address>(m->addr)+")").c_str());
#endif
		DebugAssert(pendingEviction.find(m->addr) != pendingEviction.end());
		pendingEviction.erase(m->addr);
		SendMsg(localConnectionID, m, localSendTime);
      */
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
		DebugAssert(pendingRemoteInvalidates.find(m->addr) == pendingRemoteInvalidates.end());
		pendingRemoteInvalidates[m->addr].sourceNode = src;
		pendingRemoteInvalidates[m->addr].msg = m;
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
      m->blockAttached;
#endif
      DebugAssert(waitingForInvalidates.find(m->solicitingMessage)!=waitingForInvalidates.end());
      DebugAssert(waitingForInvalidates[m->solicitingMessage].count > 0);

      // subtract the pending invalidates counter. If it reaches 0, it means all
         // invalidates have been accounted for, so we can do the read now.
      waitingForInvalidates[m->solicitingMessage].count--;
      if (waitingForInvalidates[m->solicitingMessage].count==0)
      {
         const ReadResponseMsg* rrm = waitingForInvalidates[m->solicitingMessage].msg;
         SendLocalReadResponse(rrm);
      }
      waitingForInvalidates.erase(m->solicitingMessage);
      EM().DisposeMsg(m);
      /*
       * TODO did all the directoryData operations in OnDirectoryBlockRequest
		DebugAssert(!m->blockAttached || (b.owner == src) || (b.owner==InvalidNodeID));
		DebugAssert(m->blockAttached || b.sharers.find(src) != b.sharers.end());
		if(b.owner == src)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("OnRemoteInvalidateResponse",*m,"b.owner=InvalidNodeID",src);
#endif
			b.owner = InvalidNodeID;
		}
		else
		{
			DebugAssert(b.sharers.find(src) != b.sharers.end());
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
			printDebugInfo("OnRemoteInvalidateResponse",*m,
			   ("b.sharers.erase("+to_string<NodeID>(src)+")").c_str(),src);
#endif
			b.sharers.erase(src);
		}
		*/
      DebugFail("should not reach here");
		if(m->blockAttached)
		{
			WriteMsg* wm = EM().CreateWriteMsg(getDeviceID(), m->GeneratingPC());
			wm->addr = m->addr;
			wm->size = m->size;
			wm->onCompletedCallback = NULL;
			NetworkMsg* nm = EM().CreateNetworkMsg(getDeviceID(), m->GeneratingPC());
			nm->sourceNode = nodeID;
			nm->destinationNode = memoryNodeCalc->CalcNodeID(m->addr);
			nm->payloadMsg = wm;
			SendMsg(remoteConnectionID, nm, remoteSendTime);
		}
//TODO 2010/09/09 Eric

		if(pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end())
		{
		   BlockData b = BlockData();
		   if (b.owner == src)
		   {
		      b.owner = InvalidNodeID;
		      DebugAssert(b.sharers.find(src) == b.sharers.end());
		   }
		   else if (b.sharers.find(src) != b.sharers.end())
		   {
		      b.sharers.erase(src);
		   }
		   else
		   {
		      DebugFail("failed at erasing blockData");
		   }
         // send out a directory fetch, add owner to blockData, and create send a ReadResponseMsg to local
		   if ( (b.owner == InvalidNodeID) && (b.sharers.size()==0))
			//if( (b.owner == InvalidNodeID && b.sharers.size() == 0) ||
				//(b.owner == pendingDirectoryExclusiveReads[m->addr].sourceNode && b.sharers.size() == 0) ||
				//(b.owner == InvalidNodeID && b.sharers.size() == 1 && b.sharers.find(pendingDirectoryExclusiveReads[m->addr].sourceNode) != b.sharers.end()))
			{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
			   printDebugInfo("OnRemoteInvalidateResponse",*m,"b.sharers.clear()",src);
            printDebugInfo("OnRemoteInvalidateResponse",*m,
               ("b.owner="+to_string<NodeID>(pendingDirectoryExclusiveReads[m->addr].sourceNode)).c_str(),
               src);
#endif
				//b.sharers.clear();
				//b.owner = pendingDirectoryExclusiveReads[m->addr].sourceNode;

				ReadResponseMsg* rm = EM().CreateReadResponseMsg(getDeviceID(),pendingDirectoryExclusiveReads[m->addr].msg->GeneratingPC());
				rm->addr = m->addr;
				rm->size = m->size;
				rm->blockAttached = true;
				rm->solicitingMessage = pendingDirectoryExclusiveReads[m->addr].msg->MsgID();
				rm->directoryLookup = true;
				rm->exclusiveOwnership = true;
				rm->satisfied = true;
				if(pendingDirectoryExclusiveReads[m->addr].sourceNode == nodeID)
				{
					SendMsg(localConnectionID, rm, localSendTime + satisfyTime);
				}
				else
				{
					NetworkMsg* n = EM().CreateNetworkMsg(getDeviceID(), pendingDirectoryExclusiveReads[m->addr].msg->GeneratingPC());
					n->sourceNode = nodeID;
					n->destinationNode = pendingDirectoryExclusiveReads[m->addr].sourceNode;
					n->payloadMsg = rm;
					SendMsg(remoteConnectionID, n, remoteSendTime + satisfyTime);
				}

            //TODO 2010/09/09 Eric
            // do not do the following, already did it above
            //PerformDirectoryFetchOwner(pendingDirectoryExclusiveReads[m->addr].msg,pendingDirectoryExclusiveReads[m->addr].sourceNode);
				AddDirectoryShare(m->addr,pendingDirectoryExclusiveReads[m->addr].sourceNode,true);
				EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_EXCLUSIVE_READS
            printDebugInfo("OnRemoteInvalidateResponse",*m,("PDER.erase("
               +to_string<Address>(m->addr)+")").c_str(),src);
#endif
				pendingDirectoryExclusiveReads.erase(m->addr);
			}
		}//if(pendingDirectoryExclusiveReads.find(m->addr) != pendingDirectoryExclusiveReads.end())

		EM().DisposeMsg(m);
	}  // OnRemoteInvalidateResponse

	void ThreeStageDirectory::OnDirectoryBlockRequest(const BaseMsg* msgIn, NodeID src)
	{
		DebugAssert(msgIn);
      DebugAssert(msgIn->Type()==mt_Read);
      ReadMsg* m = (ReadMsg*)msgIn;
      DebugAssert(m->directoryLookup==true);
#if defined DEBUG && defined _WIN32
      MessageID tempMessageID = m->MsgID();
#endif
		DebugAssert(directoryNodeCalc->CalcNodeID(m->addr)==nodeID);
      
      bool isDirectoryBusy = (pendingDirectorySharedReads.find(m->addr)!=pendingDirectorySharedReads.end()
         || pendingDirectoryExclusiveReads.find(m->addr)!=pendingDirectoryExclusiveReads.end());
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
			return;
		} // endif cannot satisfy request at this time

      // if we are doing a shared read request
      if (!m->requestingExclusive)
      {
         OnDirectoryBlockRequestSharedRead(m,src);
         return;
      }
      else // m->requestingExclusive
      {
         OnDirectoryBlockRequestExclusiveRead(m,src);
         return;
      } //else m->requestingExclusive

		/////////////////////////// FROM OnRemoteReadResponse ////////////////////
		/*
      {
         DebugAssert(m->exclusiveOwnership);
         DebugAssert(m->blockAttached);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_DIRECTORY_DATA
         printDebugInfo("OnRemoteReadResponse", *m,
               ("directoryData[m->addr].owner="+to_string<NodeID>(
                     directoryData[m->addr].owner)).c_str(), src);
#endif
         DebugAssert(directoryData[m->addr].owner == InvalidNodeID || directoryData[m->addr].owner == src);
         if(directoryData[m->addr].sharers.size() == 0)
         {//send block on now
            ReadResponseMsg* response = (ReadResponseMsg*)EM().ReplicateMsg(m);
            response->directoryLookup = true;

            if(pendingDirectoryExclusiveReads[m->addr].sourceNode != nodeID)
            {
               NetworkMsg* net = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
               net->sourceNode = nodeID;
               net->destinationNode = pendingDirectoryExclusiveReads[m->addr].sourceNode;
               net->payloadMsg = response;
               SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
            }
            else
            {
               SendMsg(localConnectionID, response, lookupTime + localSendTime);
            }

            AddDirectoryShare(m->addr,pendingDirectoryExclusiveReads[m->addr].sourceNode,true);
            EM().DisposeMsg(pendingDirectoryExclusiveReads[m->addr].msg);
            pendingDirectoryExclusiveReads.erase(m->addr);
         } // if(directoryData[m->addr].sharers.size() == 0)
         else  // if directory has sharers
         {//hold the block, send on once all invalidations are complete
            for(HashSet<NodeID>::iterator i = directoryData[m->addr].sharers.begin(); i != directoryData[m->addr].sharers.end(); i++)
            {
               InvalidateMsg* inv = EM().CreateInvalidateMsg(getDeviceID(),m->GeneratingPC());
               inv->addr = m->addr;
               inv->size = m->size;
               if(*i != nodeID)
               {
                  NetworkMsg* net = EM().CreateNetworkMsg(getDeviceID(),m->GeneratingPC());
                  net->destinationNode = *i;
                  net->sourceNode = nodeID;
                  net->payloadMsg = inv;
                  SendMsg(remoteConnectionID, net, lookupTime + remoteSendTime);
               }
               else
               {
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
            printDebugInfo("OnRemoteInvalidate",*inv,"OnRemoteReadResponse",nodeID);
#endif
                  OnRemoteInvalidate(inv, nodeID);
               }
            }
         } // else (directoryData[m->addr].sharers.size() != 0)
      } // else pendingDirectorySharedReads.find(m->addr) == pendingDirectorySharedReads.end())
      //EM().DisposeMsg(m);
*/
      /////////////////// END FROM OnRemoteReadResponse ////////////////
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
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkRequestSharedRead","OnDirBlkResponse");
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
      else if (b.owner==InvalidNodeID && b.sharers.size()!=0)
      {// if directory state is shared, add requesting node to sharer and request reply from any sharer
         // perform directory fetch before modifying directoryData
         PerformDirectoryFetch(m,src,false,*(b.sharers.begin()));
         DebugAssert(b.sharers.find(src)==b.sharers.end());
         b.sharers.insert(src);
         // do not dispose msg here, because we are forwarding it
         //EM().DisposeMsg(m);
         return;
      }// else if (b.owner==InvalidNodeID && b.sharers.size()!=0)
      else if (b.owner!=InvalidNodeID&&b.owner!=src)
      {// if directory state is Exclusive with another owner, transitions to Busy-shared with
         // requestor as owner and send out an intervention shared request to the previous
         // owner and a speculative reply to the requestor
         DebugAssert(pendingDirectorySharedReads.find(m->addr)==pendingDirectorySharedReads.end());

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
         pendingDirectorySharedReads[m->addr].sourceNode = src;
         pendingDirectorySharedReads[m->addr].msg = read;
         AutoDetermineDestSendMsg(read,previousOwner,localSendTime,
            &ThreeStageDirectory::OnRemoteRead,"OnDirBlkReqShRead","OnRemoteRead");

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
               &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkRequestExRead","OnDirBlkResponse");
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
      else if (b.owner==InvalidNodeID && b.sharers.size()!=0)
      {// if directory state is shared, transition to Exclusive and a exclusive reply with invalidates pending
         // is returned to the requestor. Invalidations are sent to the sharers.
         DebugAssert(b.sharers.find(src)==b.sharers.end());

         // send exclusive reply with invalidates pending
         ReadResponseMsg* reply = EM().CreateReadResponseMsg(getDeviceID());
         reply->addr = m->addr;
         reply->blockAttached = false;
         reply->directoryLookup = true;
         reply->exclusiveOwnership = true;
         reply->pendingInvalidates = b.sharers.size();
         reply->isIntervention = false;
         reply->originalRequestingNode = m->originalRequestingNode;
         reply->satisfied = true;
         reply->size = m->size;
         reply->solicitingMessage = m->MsgID();
         //PerformDirectoryFetch(m,src,false,*(b.sharers.begin()));
         AutoDetermineDestSendMsg(reply,src,lookupTime+remoteSendTime,
            &ThreeStageDirectory::OnDirectoryBlockResponse,"OnDirBlkReqExRead","OnDirBlkResponse");

         // send invalidations to the sharers
         for (HashSet<NodeID>::iterator i = b.sharers.begin(); i!=b.sharers.end(); i++)
         {
            InvalidateMsg* invalidation = EM().CreateInvalidateMsg(getDeviceID());
            invalidation->addr = m->addr;
            invalidation->newOwner = src;
            invalidation->size = m->size;
            AutoDetermineDestSendMsg(invalidation,*i,lookupTime+remoteSendTime,
               &ThreeStageDirectory::OnRemoteInvalidate,"OnDirBlkReqExRead","OnRemoteInv");
         }

         b.sharers.clear();
         b.owner = src;
         // do not dispose msg here, because it is still in pendingLocalReads
         //EM().DisposeMsg(m);
         return;
      }// else if (b.owner==InvalidNodeID && b.sharers.size()!=0)
      else if (b.owner!=InvalidNodeID&&b.owner!=src)
      {// if directory state is Exclusive with another owner, transitions to Exclusive-shared with
         // requestor as owner and send out an intervention exclusive request to the previous
         // owner and a speculative reply to the requestor

         // size should be 0 if we don't allow the owner state?
         DebugAssert(b.sharers.size()==0);
         DebugAssert(pendingDirectoryExclusiveReads.find(m->addr)==pendingDirectoryExclusiveReads.end());

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
         pendingDirectoryExclusiveReads[m->addr].sourceNode = src;
         pendingDirectoryExclusiveReads[m->addr].msg = read;
         AutoDetermineDestSendMsg(read,previousOwner,lookupTime+remoteSendTime,
            &ThreeStageDirectory::OnRemoteRead,"OnDirBlkReqExRead","OnRemoteRead");

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
      DebugAssert(m->directoryLookup==true);
#if defined DEBUG && defined _WIN32
      MessageID tempMessageID = m->MsgID();
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_DIRECTORY_SHARED_READS
		printPendingDirectorySharedReads();
#endif
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		printPendingLocalReads();
#endif
		// check that m->solicitingMessage is in pendingLocalReads before accessing it.
		// Error here probably means that the directoryBlockResponse was sent twice or 
      // was sent to the wrong node
      DebugAssert(pendingLocalReads.find(m->solicitingMessage) != pendingLocalReads.end());
		const ReadMsg* ref = pendingLocalReads[m->solicitingMessage];

		if(!m->satisfied)
		{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_PENDING_LOCAL_READS
		   printDebugInfo("OnDirectoryBlockResponse",*m,
		         ("pendingLocalReads.erase("+to_string<MessageID>(m->solicitingMessage)+")").c_str());
#endif
			pendingLocalReads.erase(m->solicitingMessage);
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnLocalRead",*m,"OnDirectoryBlockResponse");
#endif
			OnLocalRead(ref);
			return;
		}
      // m is satisfied

      // check if we have to wait for invalidates
      if (m->pendingInvalidates != INVALID_PENDING_INVALIDATES)
      {// there are pending invalidates, so put this reply into a queue
         DebugAssert(waitingForInvalidates.find(m->MsgID())==waitingForInvalidates.end());
         waitingForInvalidates[m->MsgID()].msg = m;
         waitingForInvalidates[m->MsgID()].count = m->pendingInvalidates;
         return;
      }

      // else, send local read response to cache above
      SendLocalReadResponse(m);
	}

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
	}
	/**
	 * this is used for checkpoint purposes
	 */
	void ThreeStageDirectory::DumpRunningState(RootConfigNode& node)
	{}
	/**
	 * put anything here that you might want to output to the terminal
	 */
	void ThreeStageDirectory::DumpStats(std::ostream& out)
	{
	   std::cout << "My name is Eric" << std::endl;
	   out << "My name is Eric" << std::endl;
	}
	/**
	 * Handles all the incoming messages from outside of the directory.
	 * The message can come from the cache side or from the network.
	 */
	void ThreeStageDirectory::RecvMsg(const BaseMsg* msg, int connectionID)
	{
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
               printDebugInfo("OnLocalReadResponse",*msg,"RecvMsg");
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
               printDebugInfo("OnDirectoryBlockRequest",*m,"RecvMsg",src);
#endif
						OnDirectoryBlockRequest(m,src);
					}
					else
					{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteRead",*m,"RecvMsg",src);
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
               printDebugInfo("OnDirectoryBlockResponse",*m,"RecvMsg",src);
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
               printDebugInfo("OnRemoteReadResponse",*m,"RecvMsg",src);
#endif
						OnRemoteReadResponse(m,src);
					}
					break;
				}
			case(mt_WriteResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteWriteResponse",*payload,"RecvMsg",src);
#endif
					OnRemoteWriteResponse(payload,src);
					break;
				}
			case(mt_Eviction):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteEviction",*payload,"RecvMsg",src);
#endif
					OnRemoteEviction(payload,src);
					break;
				}
			case(mt_EvictionResponse):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteEvictionResponse",*payload,"RecvMsg",src);
#endif
					OnRemoteEvictionResponse(payload,src);
					break;
				}
			case(mt_EvictionBusyAck):
				{
#ifdef MEMORY_3_STAGE_DIRECTORY_DEBUG_VERBOSE
               printDebugInfo("OnRemoteEvictionBusyAck",*payload,"RecvMsg",src);
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
			default:
				DebugFail("Payload type unrecognized");
			}
		}
		else
		{
			DebugFail("Connection not a valid ID");
		}
	}

	/**
	 * readMsgArray in this method could be coupled with Eclipse's debugger to
	 * see what elements are in pendingDirectorySharedReads. The reason for using
	 * a regular array instead of a vector is because Eclipse can view
	 * Array elements directly, but not elements in STL containers.
	 */
	void ThreeStageDirectory::printPendingDirectorySharedReads()
	{
	   HashMap<Address, LookupData<ReadMsg> >::const_iterator myIterator;


	   myIterator = pendingDirectorySharedReads.begin();
      cout << "ThreeStageDirectory::printPendingDirectorySharedReads: ";

      int size = pendingDirectorySharedReads.size();
      cout << "size=" << size << endl;
      ReadMsg const** readMsgArray = new ReadMsg const*[size];
      //readMsgVector[0] = new ReadMsg const;

      int i = 0;
	   for (myIterator = pendingDirectorySharedReads.begin();
	         myIterator != pendingDirectorySharedReads.end(); myIterator++)
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
            << " dst=" << setw(2) << getDeviceID()
            << setw(10) << " "   // account for spacing from msgID
            << " addr=" << addr
            << " 3SDir:" << fromMethod
            << " " << operation << "(" << id << ")"
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
